// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#include "gles2_hardwareconfig.h"
#include "gles2_devicemgr.h"
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
		MATERIALSYSTEM_HARDWARECONFIG_INTERFACE_VERSION, s_HardwareConfig)

CHardwareConfig::CHardwareConfig() {
	memset(&m_Caps, 0, sizeof(m_Caps));
}

bool CHardwareConfig::CheckGLExtension(const char *extensionString, const char *extensionName) {
	size_t length = strlen(extensionName);
	const char *searchIn = extensionString, *found;
	while ((found = strstr(searchIn, extensionName)) != NULL) {
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
	if (glesVersionString != NULL && sscanf(glesVersionString, "OpenGL ES %d.%d", &glesVersionMajor, glesVersionMinor) >= 2) {
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
		m_Caps.m_Ext_ColorBufferHalfFloat = true;
		m_Caps.m_Ext_Depth24 = true;
		m_Caps.m_Ext_PackedDepthStencil = true;
		m_Caps.m_Ext_TextureFloat = true;
		m_Caps.m_Ext_TextureFloatLinear = true;
		m_Caps.m_Ext_TextureHalfFloat = true;
		m_Caps.m_Ext_TextureHalfFloatLinear = true;
		m_Caps.m_Ext_TextureNPOT = true;
	} else {
		m_Caps.m_Ext_ColorBufferHalfFloat = CheckGLExtension(glesExtensions, "GL_EXT_color_buffer_half_float");
		m_Caps.m_Ext_Depth24 = CheckGLExtension(glesExtensions, "GL_OES_depth24");
		m_Caps.m_Ext_PackedDepthStencil = CheckGLExtension(glesExtensions, "GL_OES_packed_depth_stencil");
		m_Caps.m_Ext_TextureFloat = CheckGLExtension(glesExtensions, "GL_OES_texture_float");
		m_Caps.m_Ext_TextureFloatLinear = m_Caps.m_Ext_TextureFloat &&
				CheckGLExtension(glesExtensions, "GL_OES_texture_float_linear");
		m_Caps.m_Ext_TextureHalfFloat = CheckGLExtension(glesExtensions, "GL_OES_texture_half_float");
		m_Caps.m_Ext_TextureHalfFloatLinear = m_Caps.m_Ext_TextureHalfFloat &&
				CheckGLExtension(glesExtensions, "GL_OES_texture_half_float_linear");
		m_Caps.m_Ext_TextureNPOT = CheckGLExtension(glesExtensions, "GL_OES_texture_npot");
	}
	m_Caps.m_Ext_BGRA = CheckGLExtension(glesExtensions, "GL_EXT_bgra");
	m_Caps.m_Ext_TextureCompressionS3TC = CheckGLExtension(glesExtensions, "GL_EXT_texture_compression_s3tc") ||
			CheckGLExtension(glesExtensions, "GL_NV_texture_compression_s3tc");
	m_Caps.m_Ext_TextureFormatBGRA8888 = CheckGLExtension(glesExtensions, "GL_EXT_texture_format_BGRA8888");
	m_Caps.m_Ext_TextureFilterAnisotropic = CheckGLExtension(glesExtensions, "GL_EXT_texture_filter_anisotropic");

	// Maximum values.
	m_Caps.m_MaxAnisotropy = 1;
	if (m_Caps.m_Ext_TextureFilterAnisotropic) {
		g_pGL->GetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &m_Caps.m_MaxAnisotropy);
	}
	g_pGL->GetIntegerv(GL_MAX_TEXTURE_SIZE, &m_Caps.m_MaxTextureSize);
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

int	TextureMemorySize() const {
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

int	CHardwareConfig::MaxUserClipPlanes() const {
	return 0;
}

int CHardwareConfig::GetDXSupportLevel() const {
	return m_Caps.m_nMaxDXSupportLevel;
}

bool CHardwareConfig::SupportsHDR() const {
	AssertMsg(0, "Supports HDR is deprecated, use GetHDRType instead.");
	return false;
}

HDRType_t CHardwareConfig::GetHardwareHDRType() const {
	if (m_Caps.m_Ext_ColorBufferHalfFloat && m_Caps.m_Ext_TextureHalfFloatLinear) {
		return HDR_TYPE_FLOAT;
	}
	// No 16-bit normalized format on GLES.
	return HDR_TYPE_NONE;
}

int CHardwareConfig::StencilBufferBits() const {
	return g_pShaderDeviceMgr->StencilBufferBits();
}

int CHardwareConfig::MaxViewports() const {
	// Requires NV_viewport_array, but never used in Source 2007, and was only planned for ShaderAPIDX10.
	return 1;
}

bool CHardwareConfig::SupportsHDRMode(HDRType_t nHDRMode) const {
	return (nHDRMode == GetHardwareHDRType() || nHDRMode == HDR_TYPE_NONE);
}
