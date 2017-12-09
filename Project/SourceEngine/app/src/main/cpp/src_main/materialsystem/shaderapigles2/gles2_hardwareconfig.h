// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#ifndef SHADERAPI_GLES2_HARDWARECONFIG_H
#define SHADERAPI_GLES2_HARDWARECONFIG_H

#include "IHardwareConfigInternal.h"

struct HardwareCaps_t : public MaterialAdapterInfo_t {
	int m_GLESVersion; // 200, 300, 310...

	int m_MaxAnisotropy;
	int m_MaxTextureSize;
	int m_MaxTextureSize3D;
	int m_NumVertexShaderConstants;
	int m_NumPixelShaderConstants;

	int m_MaxBlendMatrices;

	unsigned char m_Ext_BGRA : 1;
	unsigned char m_Ext_ColorBufferHalfFloat : 1;
	unsigned char m_Ext_Depth24 : 1;
	unsigned char m_Ext_DepthNonlinear : 1;
	unsigned char m_Ext_DepthTexture : 1;
	unsigned char m_Ext_FragmentPrecisionHigh : 1;
	unsigned char m_Ext_PackedDepthStencil : 1;
	unsigned char m_Ext_ShadowSamplers : 1;
	unsigned char m_Ext_Texture3D : 1;
	unsigned char m_Ext_TextureCompressionS3TC : 1;
	unsigned char m_Ext_TextureFilterAnisotropic : 1;
	unsigned char m_Ext_TextureFloat : 1;
	unsigned char m_Ext_TextureFloatLinear : 1; // Implies TextureFloat.
	unsigned char m_Ext_TextureFormatBGRA8888 : 1;
	unsigned char m_Ext_TextureHalfFloat : 1;
	unsigned char m_Ext_TextureHalfFloatLinear : 1; // Implies TextureHalfFloat.
	unsigned char m_Ext_TextureNPOT : 1;
};

class CHardwareConfig : public IHardwareConfigInternal {
public:
	CHardwareConfig();

	void SetupHardwareCaps();

	const HardwareCaps_t &Caps() const { return m_Caps; }
	HardwareCaps_t &CapsForEdit() { return m_Caps; }

	virtual bool HasDestAlphaBuffer() const;
	virtual bool HasStencilBuffer() const;
	virtual int GetFrameBufferColorDepth() const;
	// virtual int  GetSamplerCount() const;
	virtual bool HasSetDeviceGammaRamp() const;
	virtual bool SupportsCompressedTextures() const;
	// virtual VertexCompressionType_t SupportsCompressedVertices() const;
	// virtual bool SupportsNormalMapCompression() const;
	virtual bool SupportsBorderColor() const;
	virtual bool SupportsFetch4() const;
	virtual bool SupportsVertexAndPixelShaders() const;
	virtual bool SupportsPixelShaders_1_4() const;
	virtual bool SupportsPixelShaders_2_0() const;
	virtual bool SupportsVertexShaders_2_0() const;
	virtual int MaximumAnisotropicLevel() const;
	virtual int MaxTextureWidth() const;
	virtual int MaxTextureHeight() const;
	virtual int TextureMemorySize() const;
	// virtual bool SupportsOverbright() const;
	virtual bool SupportsCubeMaps() const;
	virtual bool SupportsMipmappedCubemaps() const;
	virtual bool SupportsNonPow2Textures() const;
	virtual int GetTextureStageCount() const;
	virtual int NumVertexShaderConstants() const;
	virtual int NumBooleanVertexShaderConstants() const;
	virtual int NumIntegerVertexShaderConstants() const;
	virtual int NumPixelShaderConstants() const;
	virtual int NumBooleanPixelShaderConstants() const;
	virtual int NumIntegerPixelShaderConstants() const;
	// virtual int MaxNumLights() const;
	// virtual bool SupportsHardwareLighting() const;
	virtual int MaxBlendMatrices() const;
	virtual int MaxBlendMatrixIndices() const;
	virtual int MaxTextureAspectRatio() const;
	virtual int MaxVertexShaderBlendMatrices() const;
	virtual int MaxUserClipPlanes() const;
	virtual bool UseFastClipping() const;
	virtual int GetDXSupportLevel() const;
	// virtual const char *GetShaderDLLName() const;
	// virtual bool ReadPixelsFromFrontBuffer() const;
	virtual bool PreferDynamicTextures() const;
	virtual bool SupportsHDR() const;
	// virtual bool HasProjectedBumpEnv() const;
	// virtual bool SupportsSpheremapping() const;
	// virtual bool NeedsAAClamp() const;
	// virtual bool NeedsATICentroidHack() const;
	// virtual bool SupportsColorOnSecondStream() const;
	// virtual bool SupportsStaticPlusDynamicLighting() const;
	// virtual bool PreferReducedFillrate() const;
	// virtual int GetMaxDXSupportLevel() const;
	virtual bool SpecifiesFogColorInLinearSpace() const;
	// virtual bool SupportsSRGB() const;
	// virtual bool IsAAEnabled() const;
	// virtual int GetVertexTextureCount() const;
	// virtual int GetMaxVertexTextureDimension() const;
	virtual int MaxTextureDepth() const;
	// virtual HDRType_t GetHDRType() const;
	virtual HDRType_t GetHardwareHDRType() const;
	virtual bool SupportsPixelShaders_2_b() const;
	// virtual bool SupportsShaderModel_3_0() const;
	virtual bool SupportsStreamOffset() const;
	virtual int StencilBufferBits() const;
	virtual int MaxViewports() const;
	virtual void OverrideStreamOffsetSupport(bool bOverrideEnabled, bool bEnableSupport);
	// virtual int GetShadowFilterMode() const;
	virtual int NeedsShaderSRGBConversion() const;
	virtual bool UsesSRGBCorrectBlending() const;
	virtual bool HasFastVertexTextures() const;
	virtual int MaxHWMorphBatchCount() const;
	virtual bool ActuallySupportsPixelShaders_2_b() const;
	virtual bool SupportsHDRMode(HDRType_t nHDRMode) const;

	// virtual bool GetHDREnabled() const { return m_bHDREnabled; }
	// virtual void SetHDREnabled(bool bEnable) { m_bHDREnabled = bEnable; }

	// Internal methods.

	static bool CheckGLExtension(const char *extensionString, const char *extensionName);

private:
	HardwareCaps_t m_Caps;
};

extern CHardwareConfig *g_pHardwareConfig;

#endif
