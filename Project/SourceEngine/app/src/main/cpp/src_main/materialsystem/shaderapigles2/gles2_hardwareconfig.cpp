// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#include "gles2_hardwareconfig.h"
#include "gles2_devicemgr.h"
#include "gles2_glfuncs.h"
#include "gles2_mesh.h" // For SHADERAPI_GLES2_HARDWARE_MORPH.
#include "materialsystem/imaterialsystem.h"
#include "materialsystem/IShader.h"
#include "tier1/strtools.h"
#include <stdio.h>
#include <string.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static CHardwareConfig s_HardwareConfig;
CHardwareConfig *g_pHardwareConfig = &s_HardwareConfig;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CHardwareConfig, IMaterialSystemHardwareConfig,
		MATERIALSYSTEM_HARDWARECONFIG_INTERFACE_VERSION, s_HardwareConfig)

CHardwareConfig::CHardwareConfig() {
	memset(&m_Caps, 0, sizeof(m_Caps));
}

bool CHardwareConfig::CheckGLExtension(const char *extensionString, const char *extensionName) {
	size_t length = strlen(extensionName);
	const char *searchIn = extensionString, *found;
	while ((found = strstr(searchIn, extensionName)) != nullptr) {
		if ((found == extensionString || found[-1] == ' ') &&
				(found[length] == ' ' || found[length] == '\0')) {
			return true;	
		}
		searchIn += length;
	}
	return false;
}

void CHardwareConfig::SetupHardwareCaps() {
	// Vendor and renderer.
	Q_snprintf(m_Caps.m_pDriverName, sizeof(m_Caps.m_pDriverName), "%s %s",
			g_pGL->GetString(GL_VENDOR), g_pGL->GetString(GL_RENDERER));

	// Version.
	const char *glesVersionString = g_pGL->GetString(GL_VERSION);
	unsigned int glesVersionMajor, glesVersionMinor;
	if (glesVersionString != nullptr && sscanf(glesVersionString, "OpenGL ES %d.%d", &glesVersionMajor, glesVersionMinor) >= 2) {
		m_Caps.m_GLESVersion = glesVersionMajor * 100 + glesVersionMinor * 10;
		if (m_Caps.m_GLESVersion < 200) {
			m_Caps.m_GLESVersion = 200;
		}
	} else {
		m_Caps.m_GLESVersion = 200;
	}
	// Normalization cubemap is created in CTextureManager for >= 90, and GLES 2 doesn't support signed textures.
	m_Caps.m_nMaxDXSupportLevel = (m_Caps.m_GLESVersion >= 300 ? 90 : 81);
	m_Caps.m_nDXSupportLevel = m_Caps.m_nMaxDXSupportLevel;

	// Extensions.
	const char *glesExtensions = g_pGL->GetString(GL_EXTENSIONS);
	if (m_Caps.m_GLESVersion >= 300) {
		m_Caps.m_Ext_ColorBufferHalfFloat = 1;
		m_Caps.m_Ext_Depth24 = 1;
		m_Caps.m_Ext_DepthTexture = 1;
		m_Caps.m_Ext_FragmentPrecisionHigh = 1;
		m_Caps.m_Ext_PackedDepthStencil = 1;
		m_Caps.m_Ext_ShadowSamplers = 1;
		m_Caps.m_Ext_Texture3D = 1;
		m_Caps.m_Ext_TextureFloat = 1;
		m_Caps.m_Ext_TextureFloatLinear = 1;
		m_Caps.m_Ext_TextureHalfFloat = 1;
		m_Caps.m_Ext_TextureHalfFloatLinear = 1;
		m_Caps.m_Ext_TextureNPOT = 1;
	} else {
		m_Caps.m_Ext_ColorBufferHalfFloat = CheckGLExtension(glesExtensions, "GL_EXT_color_buffer_half_float");
		int fragmentHighFloatRange[2], fragmentHighFloatPrecision = 0;
		g_pGL->GetShaderPrecisionFormat(GL_FRAGMENT_SHADER, GL_HIGH_FLOAT, fragmentHighFloatRange, &fragmentHighFloatPrecision);
		m_Caps.m_Ext_FragmentPrecisionHigh = (fragmentHighFloatPrecision > 0);
		m_Caps.m_Ext_Depth24 = CheckGLExtension(glesExtensions, "GL_OES_depth24");
		m_Caps.m_Ext_DepthTexture = CheckGLExtension(glesExtensions, "GL_OES_depth_texture");
		m_Caps.m_Ext_PackedDepthStencil = CheckGLExtension(glesExtensions, "GL_OES_packed_depth_stencil");
		m_Caps.m_Ext_Texture3D = CheckGLExtension(glesExtensions, "GL_OES_texture_3D");
		m_Caps.m_Ext_TextureFloat = CheckGLExtension(glesExtensions, "GL_OES_texture_float");
		m_Caps.m_Ext_TextureHalfFloat = CheckGLExtension(glesExtensions, "GL_OES_texture_half_float");
		m_Caps.m_Ext_TextureNPOT = CheckGLExtension(glesExtensions, "GL_OES_texture_npot");
		// Dependencies.
		m_Caps.m_Ext_TextureHalfFloatLinear = m_Caps.m_Ext_TextureHalfFloat &&
				CheckGLExtension(glesExtensions, "GL_OES_texture_half_float_linear");
		m_Caps.m_Ext_ShadowSamplers = m_Caps.m_Ext_DepthTexture &&
				CheckGLExtension(glesExtensions, "GL_EXT_shadow_samplers");
	}
	m_Caps.m_Ext_BGRA = CheckGLExtension(glesExtensions, "GL_EXT_bgra");
	m_Caps.m_Ext_DepthNonlinear = CheckGLExtension(glesExtensions, "GL_NV_depth_nonlinear");
	m_Caps.m_Ext_TextureCompressionS3TC = CheckGLExtension(glesExtensions, "GL_EXT_texture_compression_s3tc") ||
			CheckGLExtension(glesExtensions, "GL_NV_texture_compression_s3tc");
	m_Caps.m_Ext_TextureFormatBGRA8888 = CheckGLExtension(glesExtensions, "GL_EXT_texture_format_BGRA8888");
	m_Caps.m_Ext_TextureFilterAnisotropic = CheckGLExtension(glesExtensions, "GL_EXT_texture_filter_anisotropic");

	// Maximum values.

	if (m_Caps.m_GLESVersion >= 300) {
		m_Caps.m_MaxTextureSize = 2048;
		m_Caps.m_MaxTextureSizeCubemap = 2048;
		m_Caps.m_MaxTextureSize3D = 256;
		m_Caps.m_NumVertexShaderConstants = 256;
		m_Caps.m_NumPixelShaderConstants = 224;
	} else {
		m_Caps.m_MaxTextureSize = 64;
		m_Caps.m_MaxTextureSizeCubemap = 16;
		m_Caps.m_MaxTextureSize3D = 0;
		m_Caps.m_NumVertexShaderConstants = 128;
		m_Caps.m_NumPixelShaderConstants = 16;
	}
	m_Caps.m_MaxAnisotropy = 1;

	if (m_Caps.m_Ext_TextureFilterAnisotropic) {
		g_pGL->GetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &m_Caps.m_MaxAnisotropy);
	}
	g_pGL->GetIntegerv(GL_MAX_TEXTURE_SIZE, &m_Caps.m_MaxTextureSize);
	g_pGL->GetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &m_Caps.m_MaxTextureSizeCubemap);
	if (m_Caps.m_Ext_Texture3D) {
		g_pGL->GetIntegerv(GL_MAX_3D_TEXTURE_SIZE_OES, &m_Caps.m_MaxTextureSize3D);
	}
	g_pGL->GetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &m_Caps.m_NumVertexShaderConstants);
	g_pGL->GetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &m_Caps.m_NumPixelShaderConstants);

	if (m_Caps.m_NumVertexShaderConstants >= VERTEX_SHADER_MODEL + 3 * NUM_MODEL_TRANSFORMS) {
		// The smallest in the near-GLES3 (256) zone is 251 on Adreno 2xx.
		m_Caps.m_MaxVertexShaderBlendMatrices = NUM_MODEL_TRANSFORMS;
	} else {
		m_Caps.m_MaxVertexShaderBlendMatrices = 16;
	}
}

bool CHardwareConfig::HasDestAlphaBuffer() const {
	return true;
}

bool CHardwareConfig::HasStencilBuffer() const {
	return StencilBufferBits() > 0;
}

int	CHardwareConfig::GetFrameBufferColorDepth() const {
	return 4;
}

bool CHardwareConfig::HasSetDeviceGammaRamp() const {
	return false;
}

bool CHardwareConfig::SupportsCompressedTextures() const {
	return m_Caps.m_Ext_TextureCompressionS3TC;
}

bool CHardwareConfig::SupportsBorderColor() const {
	return false;
}

bool CHardwareConfig::SupportsFetch4() const {
	return m_Caps.m_GLESVersion >= 310;
}

bool CHardwareConfig::SupportsVertexAndPixelShaders() const {
	return true;
}

bool CHardwareConfig::SupportsPixelShaders_1_4() const {
	return true;
}

bool CHardwareConfig::SupportsPixelShaders_2_0() const {
	return true;
}

bool CHardwareConfig::SupportsVertexShaders_2_0() const {
	return true;
}

int CHardwareConfig::MaximumAnisotropicLevel() const {
	return m_Caps.m_MaxAnisotropy;
}

int CHardwareConfig::MaxTextureWidth() const {
	return m_Caps.m_MaxTextureSize;
}

int CHardwareConfig::MaxTextureHeight() const {
	return m_Caps.m_MaxTextureSize;
}

int	CHardwareConfig::TextureMemorySize() const {
	return 102236160; // 97.5 MB. No EGL/GL way of retrieval, and on Android memory is shared anyway.
}

bool CHardwareConfig::SupportsCubeMaps() const {
	return true;
}

bool CHardwareConfig::SupportsMipmappedCubemaps() const {
	return true;
}

bool CHardwareConfig::SupportsNonPow2Textures() const {
	return m_Caps.m_Ext_TextureNPOT;
}

int CHardwareConfig::GetTextureStageCount() const {
	return 0;
}

int	CHardwareConfig::NumVertexShaderConstants() const {
	return m_Caps.m_NumVertexShaderConstants;
}

int	CHardwareConfig::NumBooleanVertexShaderConstants() const {
	return NumVertexShaderConstants();
}

int	CHardwareConfig::NumIntegerVertexShaderConstants() const {
	return NumVertexShaderConstants();
}

int	CHardwareConfig::NumPixelShaderConstants() const {
	return m_Caps.m_NumPixelShaderConstants;
}

int	CHardwareConfig::NumBooleanPixelShaderConstants() const {
	return NumPixelShaderConstants();
}

int	CHardwareConfig::NumIntegerPixelShaderConstants() const {
	return NumPixelShaderConstants();
}

int	CHardwareConfig::MaxBlendMatrices() const {
	return 0;
}

int CHardwareConfig::MaxBlendMatrixIndices() const {
	return 0;
}

int CHardwareConfig::MaxTextureAspectRatio() const {
	return m_Caps.m_MaxTextureSize;
}

int	CHardwareConfig::MaxVertexShaderBlendMatrices() const {
	return m_Caps.m_MaxVertexShaderBlendMatrices;
}

int	CHardwareConfig::MaxUserClipPlanes() const {
	return 0;
}

bool CHardwareConfig::UseFastClipping() const {
	return true;
}

int CHardwareConfig::GetDXSupportLevel() const {
	return m_Caps.m_nMaxDXSupportLevel;
}

bool CHardwareConfig::PreferDynamicTextures() const {
	return true;
}

bool CHardwareConfig::SupportsHDR() const {
	AssertMsg(0, "Supports HDR is deprecated, use GetHDRType instead.");
	return false;
}

bool CHardwareConfig::SpecifiesFogColorInLinearSpace() const {
	return true;
}

int CHardwareConfig::MaxTextureDepth() const {
	return (m_Caps.m_MaxTextureSize3D > 0 ? m_Caps.m_MaxTextureSize3D : 1);
}

HDRType_t CHardwareConfig::GetHardwareHDRType() const {
	if (m_Caps.m_Ext_ColorBufferHalfFloat && m_Caps.m_Ext_TextureHalfFloatLinear) {
		return HDR_TYPE_FLOAT;
	}
	// No 16-bit normalized format on GLES.
	return HDR_TYPE_NONE;
}

bool CHardwareConfig::SupportsPixelShaders_2_b() const {
	// Don't want sRGB output in shaders due to blending.
	return false;
}

bool CHardwareConfig::SupportsStreamOffset() const {
	return true;
}

void CHardwareConfig::OverrideStreamOffsetSupport(bool bOverrideEnabled, bool bEnableSupport) {
}

int CHardwareConfig::NeedsShaderSRGBConversion() const {
	return 0;
}

bool CHardwareConfig::UsesSRGBCorrectBlending() const {
	return true;
}

bool CHardwareConfig::HasFastVertexTextures() const {
	// See SHADERAPI_GLES2_HARDWARE_MORPH.
	return false;
}

int CHardwareConfig::MaxHWMorphBatchCount() const {
	// See SHADERAPI_GLES2_HARDWARE_MORPH.
	return 0;
}

int CHardwareConfig::StencilBufferBits() const {
	return g_pShaderDeviceMgr->StencilBufferBits();
}

int CHardwareConfig::MaxViewports() const {
	// Requires NV_viewport_array, but never used in Source 2007, and was only planned for ShaderAPIDX10.
	return 1;
}

bool CHardwareConfig::ActuallySupportsPixelShaders_2_b() const {
	// Used to enable the flashlight random vectors texture, why not? It's just 32x32.
	return true;
}

bool CHardwareConfig::SupportsHDRMode(HDRType_t nHDRMode) const {
	return (nHDRMode == GetHardwareHDRType() || nHDRMode == HDR_TYPE_NONE);
}

int CHardwareConfig::MaxCubemapSize() const {
	return m_Caps.m_MaxTextureSizeCubemap;
}
