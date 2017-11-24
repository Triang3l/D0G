// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#ifndef SHADERAPI_GLES2_HARDWARECONFIG_H
#define SHADERAPI_GLES2_HARDWARECONFIG_H

#include "IHardwareConfigInternal.h"

struct HardwareCaps_t : public MaterialAdapterInfo_t {
	int m_GLESVersion; // 200, 300, 310...

	int m_MaxAnisotropy;
	int m_MaxTextureSize;

	bool m_Ext_TextureFilterAnisotropic : 1;
	bool m_Ext_TextureNPOT : 1;
};

class CHardwareConfig : public IHardwareConfigInternal {
public:
	CHardwareConfig();

	void SetupHardwareCaps();

	const HardwareCaps_t &Caps() const { return m_Caps; }
	HardwareCaps_t &CapsForEdit() { return m_Caps; }

	// virtual bool HasDestAlphaBuffer() const;
	// virtual bool HasStencilBuffer() const;
	// virtual int	 GetFrameBufferColorDepth() const;
	// virtual int  GetSamplerCount() const;
	// virtual bool HasSetDeviceGammaRamp() const;
	// virtual bool SupportsCompressedTextures() const;
	// virtual VertexCompressionType_t SupportsCompressedVertices() const;
	// virtual bool SupportsNormalMapCompression() const;
	virtual bool SupportsBorderColor() const { return false; }
	virtual bool SupportsFetch4() const { return m_Caps.m_GLESVersion >= 310; }
	virtual bool SupportsVertexAndPixelShaders() const { return true; }
	virtual bool SupportsPixelShaders_1_4() const { return true; }
	virtual bool SupportsPixelShaders_2_0() const { return true; }
	virtual bool SupportsVertexShaders_2_0() const { return true; }
	virtual int  MaximumAnisotropicLevel() const { return m_Caps.m_MaxAnisotropy; }
	virtual int  MaxTextureWidth() const { return m_Caps.m_MaxTextureSize; }
	virtual int  MaxTextureHeight() const { return m_Caps.m_MaxTextureSize; }
	// virtual int	 TextureMemorySize() const;
	// virtual bool SupportsOverbright() const;
	virtual bool SupportsCubeMaps() const { return true; }
	virtual bool SupportsMipmappedCubemaps() const { return true; }
	virtual bool SupportsNonPow2Textures() const { return m_Caps.m_Ext_TextureNPOT; }
	// virtual int  GetTextureStageCount() const;
	// virtual int	 NumVertexShaderConstants() const;
	// virtual int	 NumBooleanVertexShaderConstants() const;
	// virtual int	 NumIntegerVertexShaderConstants() const;
	// virtual int	 NumPixelShaderConstants() const;
	// virtual int	 NumBooleanPixelShaderConstants() const;
	// virtual int	 NumIntegerPixelShaderConstants() const;
	// virtual int	 MaxNumLights() const;
	// virtual bool SupportsHardwareLighting() const;
	// virtual int	 MaxBlendMatrices() const;
	// virtual int	 MaxBlendMatrixIndices() const;
	// virtual int  MaxTextureAspectRatio() const;
	// virtual int	 MaxVertexShaderBlendMatrices() const;
	virtual int	 MaxUserClipPlanes() const { return 0; }
	// virtual bool UseFastClipping() const;
	virtual int  GetDXSupportLevel() const;
	// virtual const char *GetShaderDLLName() const;
	// virtual bool ReadPixelsFromFrontBuffer() const;
	// virtual bool PreferDynamicTextures() const;
	// virtual bool SupportsHDR() const;
	// virtual bool HasProjectedBumpEnv() const;
	// virtual bool SupportsSpheremapping() const;
	// virtual bool NeedsAAClamp() const;
	// virtual bool NeedsATICentroidHack() const;
	// virtual bool SupportsColorOnSecondStream() const;
	// virtual bool SupportsStaticPlusDynamicLighting() const;
	// virtual bool PreferReducedFillrate() const;
	// virtual int	 GetMaxDXSupportLevel() const;
	// virtual bool SpecifiesFogColorInLinearSpace() const;
	// virtual bool SupportsSRGB() const;
	// virtual bool IsAAEnabled() const;
	// virtual int  GetVertexTextureCount() const;
	// virtual int  GetMaxVertexTextureDimension() const;
	// virtual int  MaxTextureDepth() const;
	// virtual HDRType_t GetHDRType() const;
	// virtual HDRType_t GetHardwareHDRType() const;
	// virtual bool SupportsPixelShaders_2_b() const;
	// virtual bool SupportsShaderModel_3_0() const;
	// virtual bool SupportsStreamOffset() const;
	// virtual int  StencilBufferBits() const;
	// virtual int  MaxViewports() const;
	// virtual void OverrideStreamOffsetSupport( bool bOverrideEnabled, bool bEnableSupport );
	// virtual int  GetShadowFilterMode() const;
	// virtual int NeedsShaderSRGBConversion() const;
	// virtual bool UsesSRGBCorrectBlending() const;
	// virtual bool HasFastVertexTextures() const;
	// virtual int MaxHWMorphBatchCount() const;

	// const char *GetHWSpecificShaderDLLName() const;
	// int GetActualSamplerCount() const;
	// int GetActualTextureStageCount() const;
	bool SupportsMipmapping() const { return true; }
	// virtual bool ActuallySupportsPixelShaders_2_b() const;

	// virtual bool SupportsHDRMode( HDRType_t nHDRMode ) const;

	// virtual bool GetHDREnabled( void ) const { return m_bHDREnabled; }
	// virtual void SetHDREnabled( bool bEnable ) { m_bHDREnabled = bEnable; }

private:
	HardwareCaps_t m_Caps;
};

extern CHardwareConfig *g_pHardwareConfig;

#endif
