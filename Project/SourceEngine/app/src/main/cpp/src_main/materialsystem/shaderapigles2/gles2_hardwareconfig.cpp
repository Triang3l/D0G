// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#include "gles2_hardwareconfig.h"
#include "gles2_glfuncs.h"
#include "materialsystem/imaterialsystem.h"
#include "tier1/strtools.h"
#include <stdio.h>
#include <string.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static CHardwareConfig s_HardwareConfig;
CHardwareConfig *g_pHardwareConfig = &s_HardwareConfig;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CHardwareConfig, IMaterialSystemHardwareConfig,
		MATERIALSYSTEM_HARDWARECONFIG_INTERFACE_VERSION, s_HardwareConfig);

CHardwareConfig::CHardwareConfig() {
	memset(&m_Caps, 0, sizeof(m_Caps));
}

void CHardwareConfig::SetupHardwareCaps() {
	// Vendor and renderer.
	Q_snprintf(m_Caps.m_pDriverName, sizeof(m_Caps.m_pDriverName), "%s %s",
			g_pGL->GetString(GL_VENDOR), g_pGL->GetString(GL_RENDERER));

	// Version.
	const char *glesVersionString = g_pGL->GetString(GL_VERSION);
	unsigned int glesVersionMajor, glesVersionMinor;
	if (glesVersionString != NULL && sscanf(glesVersionString, "OpenGL ES %d.%d", &glesVersionMajor, glesVersionMinor) >= 2) {
		m_Caps.m_GLESVersion = glesVersionMajor * 100 + glesVersionMinor * 10;
		if (m_Caps.m_GLESVersion < 200) {
			m_Caps.m_GLESVersion = 200;
		}
	} else {
		m_Caps.m_GLESVersion = 200;
	}
	m_Caps.m_nMaxDXSupportLevel = 90; // D0GTODO: 95 if GLES3 or some extensions supported?
	m_Caps.m_nDXSupportLevel = m_Caps.m_nMaxDXSupportLevel;

	// Extensions.
	const char *glesExtensions = g_pGL->GetString(GL_EXTENSIONS);
	if (m_Caps.m_GLESVersion >= 300) {
		m_Caps.m_Ext_TextureNPOT = true;
	} else {
		m_Caps.m_Ext_TextureNPOT = (strstr(glesExtensions, "GL_OES_texture_npot") != NULL);
	}
	m_Caps.m_Ext_TextureFilterAnisotropic = (strstr(glesExtensions, "GL_EXT_texture_filter_anisotropic") != NULL);

	// Maximum values.
	m_Caps.m_MaxAnisotropy = 1;
	if (m_Caps.m_Ext_TextureFilterAnisotropic) {
		g_pGL->GetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &m_Caps.m_MaxAnisotropy);
	}
	g_pGL->GetIntegerv(GL_MAX_TEXTURE_SIZE, &m_Caps.m_MaxTextureSize);
}
