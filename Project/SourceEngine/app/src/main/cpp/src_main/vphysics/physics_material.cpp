#include <stdio.h>
#include <ctype.h>
#include "physics_material.h"
#include "physics_vehicle.h"

#include "ivp_physics.hxx"
#include "ivp_material.hxx"
#include "utlsymbol.h"
#include "commonmacros.h"
#include "vstdlib/strtools.h" 
#include "vcollide_parse_private.h"

static int Limit( int value, int low, int high )
{
	if ( value < low )
		value = low;
	if ( value > high )
		value = high;

	return value;
}

//-----------------------------------------------------------------------------
// Purpose: This is the data stored for each material/surface propery list
//-----------------------------------------------------------------------------
class CSurface : public IVP_Material
{
public:
// IVP_Material
    virtual IVP_DOUBLE get_friction_factor()
	{
		return data.physics.friction;
	}
    
    virtual IVP_DOUBLE get_elasticity()
	{
		return data.physics.elasticity;
	}
    virtual const char *get_name();
	// UNDONE: not implemented here.
	virtual IVP_DOUBLE get_second_friction_factor() 
	{ 
		return 0; 
	}
    virtual IVP_DOUBLE get_adhesion()
	{
		return 0;
	}

// strings
	CUtlSymbol			m_name;
	unsigned short		m_pad;

// physics properties
	surfacedata_t	data;
};


static IVP_Material_Simple g_ShadowMaterial( 0.8f, 0.f );

inline bool IsMoveable( IVP_Real_Object *pObject )
{
	IVP_Core *pCore = pObject->get_core();
	if ( pCore->pinned || pCore->physical_unmoveable )
		return false;
	return true;
}

inline bool IVPFloatPointIsZero( const IVP_U_Float_Point &test )
{
	const float eps = 1e-4;
	return test.quad_length() < eps ? true : false;
}

class CPhysicsSurfaceProps;

class CIVPMaterialManager : public IVP_Material_Manager
{
	typedef IVP_Material_Manager BaseClass;
public:
	CIVPMaterialManager( void );
	void Init( CPhysicsSurfaceProps *pProps ) { m_props = pProps; }
	void SetPropMap( int *map, int mapSize );
	int RemapIVPMaterialIndex( int ivpMaterialIndex );

	// IVP_Material_Manager
	virtual IVP_Material *get_material_by_index(const IVP_U_Point *world_position, int index);

    virtual IVP_DOUBLE get_friction_factor(IVP_Contact_Situation *situation)	// returns values >0, value of 1.0f means object stands on a 45 degres hill
	{
		// vehicle wheels get no friction with stuff that isn't ground
		// helps keep control of the car
		// traction on less than 60 degree slopes.
		if ( fabs(situation->surf_normal.k[1]) < 0.5 )
		{
			// BUGBUG: IVP sometimes sends us a bogus normal
			// when doing a material realc on existing contacts!
			if ( !IVPFloatPointIsZero(situation->surf_normal) )
			{
				if ( (IsVehicleWheel( situation->objects[0] ) && !IsMoveable( situation->objects[1] ))
					|| (IsVehicleWheel( situation->objects[1] ) && !IsMoveable( situation->objects[0] )) )
				{
					
					//Vector tmp;ConvertDirectionToHL( situation->surf_normal, tmp );Msg("Wheel sliding on surface %.2f %.2f %.2f\n", tmp.x, tmp.y, tmp.z );
					return 0;
				}
			}
		}

		return BaseClass::get_friction_factor( situation );
	}

#if _DEBUG
    virtual IVP_DOUBLE get_elasticity(IVP_Contact_Situation *situation)		// range [0, 1.0f[, the relative speed after a collision compared to the speed before
	{
		return BaseClass::get_elasticity( situation );
	}
#endif

private:
	CPhysicsSurfaceProps	*m_props;
	unsigned short			m_propMap[128];
};


//-----------------------------------------------------------------------------
// Purpose: This is the main database of materials
//-----------------------------------------------------------------------------
class CPhysicsSurfaceProps : public IPhysicsSurfacePropsInternal
{
public:
	CPhysicsSurfaceProps( void );
	~CPhysicsSurfaceProps( void );

	virtual int		ParseSurfaceData( const char *pFilename, const char *pTextfile );
	virtual int		SurfacePropCount( void ) const;
	virtual int		GetSurfaceIndex( const char *pPropertyName ) const;
	virtual void	GetPhysicsProperties( int surfaceDataIndex, float *density, float *thickness, float *friction, float *elasticity ) const;
	virtual void	GetPhysicsParameters( int surfaceDataIndex, surfacephysicsparams_t *pParamsOut ) const;
	virtual surfacedata_t *GetSurfaceData( int surfaceDataIndex );
	virtual const char *GetString( unsigned short stringTableIndex ) const;
	virtual const char *GetPropName( int surfaceDataIndex ) const;
	virtual void SetWorldMaterialIndexTable( int *pMapArray, int mapSize );
	virtual int RemapIVPMaterialIndex( int ivpMaterialIndex )
	{
		return m_ivpManager.RemapIVPMaterialIndex( ivpMaterialIndex );
	}
	bool IsReservedMaterialIndex( int materialIndex );
	virtual IVP_Material *GetReservedMaterial( int materialIndex );
	virtual const char *GetReservedMaterialName( int materialIndex );
	int GetReservedSurfaceIndex( const char *pPropertyName );

	// The database is derived from the IVP material class
	IVP_Material *GetIVPMaterial( int materialIndex );
	virtual int GetIVPMaterialIndex( IVP_Material *pIVP );
	IVP_Material_Manager *GetIVPManager( void ) { return &m_ivpManager; }

	const char *GetNameString( CUtlSymbol name )
	{
		return m_strings.String(name);
	}

private:
	CSurface	*GetInternalSurface( int materialIndex );
	
	void			CopyPhysicsProperties( CSurface *pOut, int baseIndex );
	bool			AddFileToDatabase( const char *pFilename );

private:
	CUtlSymbolTable				m_strings;
	CUtlVector<CUtlSymbol>		m_soundList;
	CUtlVector<CSurface>		m_props;
	CUtlVector<CUtlSymbol>		m_fileList;
	CIVPMaterialManager			m_ivpManager;
};


// Singleton database object
CPhysicsSurfaceProps g_SurfaceDatabase;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CPhysicsSurfaceProps, IPhysicsSurfaceProps, VPHYSICS_SURFACEPROPS_INTERFACE_VERSION, g_SurfaceDatabase);


// Global pointer to singleton for VPHYSICS.DLL internal access
IPhysicsSurfacePropsInternal *physprops = &g_SurfaceDatabase;


const char *CSurface::get_name()
{
	return g_SurfaceDatabase.GetNameString( m_name );
}

CPhysicsSurfaceProps::CPhysicsSurfaceProps( void ) : m_fileList(8,8)
{
	m_ivpManager.Init( this );
}


CPhysicsSurfaceProps::~CPhysicsSurfaceProps( void )
{
}

int CPhysicsSurfaceProps::SurfacePropCount( void ) const
{
	return m_props.Size();
}

// Add the filename to a list to make sure each file is only processed once
bool CPhysicsSurfaceProps::AddFileToDatabase( const char *pFilename )
{
	CUtlSymbol id = m_strings.AddString( pFilename );

	for ( int i = 0; i < m_fileList.Size(); i++ )
	{
		if ( m_fileList[i] == id )
			return false;
	}

	m_fileList.AddToTail( id );
	return true;
}

int CPhysicsSurfaceProps::GetSurfaceIndex( const char *pPropertyName ) const
{
	if ( pPropertyName[0] == '$' )
	{
		int index = GetReservedSurfaceIndex( pPropertyName );
		if ( index >= 0 )
			return index;
	}

	CUtlSymbol id = m_strings.Find( pPropertyName );
	if ( id.IsValid() )
	{
		// BUGBUG: Linear search is slow!!!
		for ( int i = 0; i < m_props.Size(); i++ )
		{
			if ( m_props[i].m_name == id )
				return i;
		}
	}

	return -1;
}


const char *CPhysicsSurfaceProps::GetPropName( int surfaceDataIndex ) const
{
	if ( IsReservedMaterialIndex( surfaceDataIndex ) )
	{
		return GetReservedMaterialName( surfaceDataIndex );
	}
	CSurface *pSurface = GetInternalSurface( surfaceDataIndex );
	if ( pSurface )
	{
		return GetNameString( pSurface->m_name );
	}
	return NULL;
}


// UNDONE: move reserved materials into this table, or into a parallel table
// that gets hooked out here.
CSurface *CPhysicsSurfaceProps::GetInternalSurface( int materialIndex )
{
	if ( materialIndex < 0 || materialIndex > m_props.Size()-1 )
	{
		return NULL;
	}
	return &m_props[materialIndex];
}

void CPhysicsSurfaceProps::GetPhysicsProperties( int materialIndex, float *density, float *thickness, float *friction, float *elasticity ) const
{
	CSurface *pSurface = GetInternalSurface( materialIndex );
	if ( pSurface )
	{
		if ( friction )
			*friction = pSurface->data.physics.friction;
		if ( elasticity )
			*elasticity = pSurface->data.physics.elasticity;
		if ( density )
			*density = pSurface->data.physics.density;
		if ( thickness )
			*thickness = pSurface->data.physics.thickness;
	}
}

void CPhysicsSurfaceProps::GetPhysicsParameters( int surfaceDataIndex, surfacephysicsparams_t *pParamsOut ) const
{
	if ( !pParamsOut )
	{
		return;
	}
	CSurface *pSurface = GetInternalSurface( materialIndex );
	if ( pSurface )
	{
		*pParamsOut = pSurface->data.physics;
	}
}


surfacedata_t *CPhysicsSurfaceProps::GetSurfaceData( int materialIndex )
{
	CSurface *pSurface = GetInternalSurface( materialIndex );
	if (!pSurface)
		pSurface = GetInternalSurface( GetSurfaceIndex( "default" ) );

	assert ( pSurface );
	return &pSurface->data;
}

const char *CPhysicsSurfaceProps::GetString( unsigned short stringTableIndex ) const
{
	CUtlSymbol index = m_soundList[stringTableIndex];
	return m_strings.String( index );
}


bool CPhysicsSurfaceProps::IsReservedMaterialIndex( int materialIndex )
{
	return (materialIndex > 127) ? true : false;
}

IVP_Material *CPhysicsSurfaceProps::GetReservedMaterial( int materialIndex )
{
	switch( materialIndex )
	{
	case MATERIAL_INDEX_SHADOW:
		return &g_ShadowMaterial;
	}

	return NULL;
}

const char *CPhysicsSurfaceProps::GetReservedMaterialName( int materialIndex )
{
	// NOTE: All of these must start with '$'
	switch( materialIndex )
	{
	case MATERIAL_INDEX_SHADOW:
		return "$MATERIAL_INDEX_SHADOW";
	}

	return NULL;
}

int CPhysicsSurfaceProps::GetReservedSurfaceIndex( const char *pPropertyName )
{
	if ( !Q_stricmp( pPropertyName, "$MATERIAL_INDEX_SHADOW" ) )
	{
		return MATERIAL_INDEX_SHADOW;
	}
	return -1;
}

IVP_Material *CPhysicsSurfaceProps::GetIVPMaterial( int materialIndex )
{
	if ( IsReservedMaterialIndex( materialIndex ) )
	{
		return GetReservedMaterial( materialIndex );
	}
	return GetInternalSurface(materialIndex);
}


int CPhysicsSurfaceProps::GetIVPMaterialIndex( IVP_Material *pIVP )
{
	int index = (CSurface *)pIVP - m_props.Base();
	if ( index >= 0 && index < m_props.Size() )
		return index;

	return -1;
}

void CPhysicsSurfaceProps::CopyPhysicsProperties( CSurface *pOut, int baseIndex )
{
	CSurface *pSurface = GetInternalSurface( baseIndex );
	if ( pSurface )
	{
		pOut->data.physics = pSurface->data.physics;
		pOut->data.audio = pSurface->data.audio;
		pOut->data.sounds = pSurface->data.sounds;
		pOut->data.game.material = pSurface->data.game.material;
	}
}


int CPhysicsSurfaceProps::ParseSurfaceData( const char *pFileName, const char *pTextfile )
{
	if ( !AddFileToDatabase( pFileName ) )
	{
		return 0;
	}

	const char *pText = pTextfile;

	do
	{
		char key[MAX_KEYVALUE], value[MAX_KEYVALUE];

		pText = ParseKeyvalue( pText, key, value );
		if ( !strcmp(value, "{") )
		{
			CSurface prop;
			int baseMaterial = GetSurfaceIndex( "default" );

			memset( &prop.data, 0, sizeof(prop.data) );
			prop.m_name = m_strings.AddString( key );
			prop.data.game.material = 0;
			prop.data.game.maxSpeedFactor = 1.0f;
			prop.data.game.jumpFactor = 1.0f;
			prop.data.game.climbable = 0;
			CopyPhysicsProperties( &prop, baseMaterial );

			do
			{
				pText = ParseKeyvalue( pText, key, value );
				if ( !strcmpi( key, "}" ) )
				{
					// already in the database, don't add again
					if ( GetSurfaceIndex( m_strings.String(prop.m_name) ) >= 0 )
						break;

					m_props.AddToTail( prop );
					break;
				}
				else if ( !strcmpi( key, "base" ) )
				{
					baseMaterial = GetSurfaceIndex( value );
					CopyPhysicsProperties( &prop, baseMaterial );
				}
				else if ( !strcmpi( key, "thickness" ) )
				{
					prop.data.physics.thickness = atof(value);
				}
				else if ( !strcmpi( key, "density" ) )
				{
					prop.data.physics.density = atof(value);
				}
				else if ( !strcmpi( key, "elasticity" ) )
				{
					prop.data.physics.elasticity = atof(value);
				}
				else if ( !strcmpi( key, "friction" ) )
				{
					prop.data.physics.friction = atof(value);
				}
				else if ( !strcmpi( key, "maxspeedfactor" ) )
				{
					prop.data.game.maxSpeedFactor = atof(value);
				}
				else if ( !strcmpi( key, "jumpfactor" ) )
				{
					prop.data.game.jumpFactor = atof(value);
				}
				else if ( !strcmpi( key, "climbable" ) )
				{
					prop.data.game.climbable = atoi(value);
				}
				else if ( !strcmpi( key, "audioReflectivity" ) )
				{
					prop.data.audio.reflectivity = atof(value);
				}
				else if ( !strcmpi( key, "audioHardnessFactor" ) )
				{
					prop.data.audio.hardnessFactor = atof(value);
				}
				else if ( !strcmpi( key, "audioHardMinVelocity" ) )
				{
					prop.data.audio.hardVelocityThreshold = atof(value);
				}
				else if ( !strcmpi( key, "audioRoughnessFactor" ) )
				{
					prop.data.audio.roughnessFactor = atof(value);
				}
				else if ( !strcmpi( key, "scrapeRoughThreshold" ) )
				{
					prop.data.audio.roughThreshold = atof(value);
				}
				else if ( !strcmpi( key, "impactHardThreshold" ) )
				{
					prop.data.audio.hardThreshold = atof(value);
				}
				else if ( !strcmpi( key, "stepleft" ) )
				{
					CUtlSymbol sym = m_strings.AddString( value );
					prop.data.sounds.stepleft = m_soundList.AddToTail( sym );
				}
				else if ( !strcmpi( key, "stepright" ) )
				{
					CUtlSymbol sym = m_strings.AddString( value );
					prop.data.sounds.stepright = m_soundList.AddToTail( sym );
				}
				else if ( !strcmpi( key, "impactsoft" ) )
				{
					CUtlSymbol sym = m_strings.AddString( value );
					prop.data.sounds.impactSoft = m_soundList.AddToTail( sym );
				}
				else if ( !strcmpi( key, "impacthard" ) )
				{
					CUtlSymbol sym = m_strings.AddString( value );
					prop.data.sounds.impactHard = m_soundList.AddToTail( sym );
				}
				else if ( !strcmpi( key, "scrapesmooth" ) )
				{
					CUtlSymbol sym = m_strings.AddString( value );
					prop.data.sounds.scrapeSmooth = m_soundList.AddToTail( sym );
				}
				else if ( !strcmpi( key, "scraperough" ) )
				{
					CUtlSymbol sym = m_strings.AddString( value );
					prop.data.sounds.scrapeRough = m_soundList.AddToTail( sym );
				}
				else if ( !strcmpi( key, "bulletimpact" ) )
				{
					CUtlSymbol sym = m_strings.AddString( value );
					prop.data.sounds.bulletImpact = m_soundList.AddToTail( sym );
				}
				else if ( !strcmpi( key, "break" ) )
				{
					CUtlSymbol sym = m_strings.AddString( value );
					prop.data.sounds.breakSound = m_soundList.AddToTail( sym );
				}
				else if ( !strcmpi( key, "strain" ) )
				{
					CUtlSymbol sym = m_strings.AddString( value );
					prop.data.sounds.strainSound = m_soundList.AddToTail( sym );
				}
				else if ( !strcmpi( key, "rolling" ) )
				{
					CUtlSymbol sym = m_strings.AddString( value );
					prop.data.sounds.rolling = m_soundList.AddToTail( sym );
				}
				else if ( !strcmpi( key, "gamematerial" ) )
				{
					if ( strlen(value) == 1 && !isdigit(value[0]) )
					{
						prop.data.game.material = toupper(value[0]);
					}
					else
					{
						prop.data.game.material = atoi(value);
					}
				}
				else if ( !strcmpi( key, "dampening" ) )
				{
					prop.data.physics.dampening = atof(value);
				}
				else
				{
					// force a breakpoint
					AssertMsg2( 0, "Bad surfaceprop key %s (%s)\n", key, value );
				}
			} while (pText);
		}
	} while (pText);

	return m_props.Size();
}


void CPhysicsSurfaceProps::SetWorldMaterialIndexTable( int *pMapArray, int mapSize )
{
	m_ivpManager.SetPropMap( pMapArray, mapSize );
}

CIVPMaterialManager::CIVPMaterialManager( void ) : IVP_Material_Manager( IVP_FALSE )
{
	// by default every index maps to itself (NULL translation)
	for ( int i = 0; i < ARRAYSIZE(m_propMap); i++ )
	{
		m_propMap[i] = i;
	}
}

int CIVPMaterialManager::RemapIVPMaterialIndex( int ivpMaterialIndex )
{
	if ( ivpMaterialIndex > 127 )
		return ivpMaterialIndex;
	
	return m_propMap[ivpMaterialIndex];
}

// remap the incoming (from IVP) index and get the appropriate material
// note that ivp will only supply indices between 1 and 127
IVP_Material *CIVPMaterialManager::get_material_by_index(const IVP_U_Point *world_position, int index)
{
	return m_props->GetIVPMaterial( RemapIVPMaterialIndex(index) );
}

// Installs a LUT for remapping IVP material indices to physprop indices
// A table of the names of the materials in index order is stored with the 
// compiled bsp file.  This is then remapped dynamically without touching the
// per-triangle indices on load.  If we wanted to support multiple LUTs, it would
// be better to preprocess/remap the triangles in the collision models at load time
void CIVPMaterialManager::SetPropMap( int *map, int mapSize )
{
	// ??? just ignore any extra bits
	if ( mapSize > 128 )
	{
		mapSize = 128;
	}

	for ( int i = 0; i < mapSize; i++ )
	{
		m_propMap[i] = (unsigned short)map[i];
	}
}
