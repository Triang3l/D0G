// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#include "gles2_api.h"
#include "gles2_glfuncs.h"
#include "gles2_hardwareconfig.h"
#include "tier0/dbg.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

GLESImageFormat_t CShaderAPIGLES2::ImageFormatToGLES(ImageFormat fmt) const {
	// This doesn't care whether it's supported or not.
	const HardwareCaps_t &caps = g_pHardwareConfig->Caps();
	switch (fmt) {
	case IMAGE_FORMAT_RGBA8888:
		return GLESImageFormat_t(GL_RGBA, GL_UNSIGNED_BYTE);
	// case IMAGE_FORMAT_ABGR8888:
		// Fall back to RGBA8888.
	case IMAGE_FORMAT_RGB888:
		return GLESImageFormat_t(GL_RGB, GL_UNSIGNED_BYTE);
	case IMAGE_FORMAT_BGR888:
		// GL_EXT_bgra, fall back to RGB888.
		return GLESImageFormat_t(GL_BGR_EXT, GL_UNSIGNED_BYTE);
	// case IMAGE_FORMAT_RGB565:
		// Fall back to BGR565.
	case IMAGE_FORMAT_I8:
		return GLESImageFormat_t(GL_LUMINANCE, GL_UNSIGNED_BYTE);
	case IMAGE_FORMAT_IA88:
		return GLESImageFormat_t(GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE);
	// case IMAGE_FORMAT_P8:
		// Fall back to I8.
	case IMAGE_FORMAT_A8:
		return GLESImageFormat_t(GL_ALPHA, GL_UNSIGNED_BYTE);
	// case IMAGE_FORMAT_RGB888_BLUESCREEN:
		// Fall back to RGBA8888.
	// case IMAGE_FORMAT_BGR888_BLUESCREEN:
		// Fall back to RGBA8888.
	// case IMAGE_FORMAT_ARGB8888:
		// Fall back to RGBA8888.
	case IMAGE_FORMAT_BGRA8888:
	case IMAGE_FORMAT_BGRX8888:
		// GL_EXT_bgra or GL_EXT_texture_format_BGRA8888, fall back to RGBA8888.
		return GLESImageFormat_t(GL_BGRA_EXT, GL_UNSIGNED_BYTE);
	case IMAGE_FORMAT_DXT1:
		// GL_EXT_texture_compression_S3TC.
		return GLESImageFormat_t(GL_COMPRESSED_RGB_S3TC_DXT1_EXT, 0);
	case IMAGE_FORMAT_DXT3:
		// GL_EXT_texture_compression_S3TC.
		return GLESImageFormat_t(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 0);
	case IMAGE_FORMAT_DXT5:
		// GL_EXT_texture_compression_S3TC.
		return GLESImageFormat_t(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 0);
	case IMAGE_FORMAT_BGR565:
		return GLESImageFormat_t(GL_RGB, GL_UNSIGNED_SHORT_5_6_5);
	case IMAGE_FORMAT_BGRX5551:
	case IMAGE_FORMAT_BGRA5551:
		return GLESImageFormat_t(GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1);
	case IMAGE_FORMAT_BGRA4444:
		return GLESImageFormat_t(GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4);
	case IMAGE_FORMAT_DXT1_ONEBITALPHA:
		// GL_EXT_texture_compression_S3TC.
		return GLESImageFormat_t(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 0);
	case IMAGE_FORMAT_UV88:
		// GLES3, fall back to IA88.
		return GLESImageFormat_t(GL_RG, GL_RG8_SNORM, GL_BYTE);
	case IMAGE_FORMAT_UVWQ8888:
		// GLES3, fall back to RGBA8888.
		return GLESImageFormat_t(GL_RGBA, GL_RGBA8_SNORM, GL_BYTE);
	case IMAGE_FORMAT_RGBA16161616F:
		// GLES3 or GL_OES_texture_half_float.
		if (caps.m_GLESVersion >= 300) {
			return GLESImageFormat_t(GL_RGBA, GL_RGBA16F, GL_HALF_FLOAT);
		}
		return GLESImageFormat_t(GL_RGBA, GL_HALF_FLOAT_OES);
	// case IMAGE_FORMAT_RGBA16161616:
		// Not supported as normalized, use RGBA16161616F instead.
	// case IMAGE_FORMAT_UVLX8888:
		// Fall back to UVWQ8888.
	case IMAGE_FORMAT_R32F:
		// GLES3 or GL_OES_texture_float.
		if (caps.m_GLESVersion >= 300) {
			return GLESImageFormat_t(GL_RED, GL_R32F, GL_FLOAT);
		}
		return GLESImageFormat_t(GL_LUMINANCE, GL_FLOAT);
	case IMAGE_FORMAT_RGB323232F:
		// GLES3 or GL_OES_texture_float.
		if (caps.m_GLESVersion >= 300) {
			return GLESImageFormat_t(GL_RGB, GL_RGB32F, GL_FLOAT);
		}
		return GLESImageFormat_t(GL_RGB, GL_FLOAT);
	case IMAGE_FORMAT_RGBA32323232F:
		// GLES3 or GL_OES_texture_float.
		if (caps.m_GLESVersion >= 300) {
			return GLESImageFormat_t(GL_RGBA, GL_RGBA32F, GL_FLOAT);
		}
		return GLESImageFormat_t(GL_RGBA, GL_FLOAT);
	case IMAGE_FORMAT_GLES_D16_RB:
		return GLESImageFormat_t(GL_DEPTH_COMPONENT16, GL_UNSIGNED_SHORT);
	case IMAGE_FORMAT_GLES_D16_TEX:
		// GL_OES_depth_texture, fall back to GLES_D16_RB.
		return GLESImageFormat_t(GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT);
	case IMAGE_FORMAT_GLES_D16_NONLINEAR_RB:
		// GL_NV_depth_nonlinear, fall back to GLES_D16_RB.
	case IMAGE_FORMAT_GLES_D16_NONLINEAR_TEX:
		// GL_NV_depth_nonlinear and GL_OES_depth_texture, try to fall back to GLES_D16_TEX or GLES_D16_NONLINEAR_RB.
		return GLESImageFormat_t(GL_DEPTH_COMPONENT16_NONLINEAR_NV, GL_UNSIGNED_SHORT);
	case IMAGE_FORMAT_GLES_D24_RB:
		// GL_OES_depth24 or GLES3, try to fall back to GLES_D16_NONLINEAR_RB.
		return GLESImageFormat_t(GL_DEPTH_COMPONENT24_OES, GL_UNSIGNED_INT);
	case IMAGE_FORMAT_GLES_D24_TEX:
		// GL_OES_depth24 and GL_OES_depth_texture or GLES3, try to fall back to GLES_D16_NONLINEAR_TEX or GLES_D24_RB.
		return GLESImageFormat_t(GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
	case IMAGE_FORMAT_GLES_DST24_RB:
		// GL_OES_packed_depth_stencil or GLES3, try to fall back to GLES_D24_RB.
		return GLESImageFormat_t(GL_DEPTH24_STENCIL8_OES, GL_UNSIGNED_INT_24_8_OES);
	case IMAGE_FORMAT_GLES_DST24_TEX:
		// GL_OES_packed_depth_stencil and GL_OES_depth_texture or GLES3, try to fall back to GLES_D24_TEX or GLES_DST24_RB.
		return GLESImageFormat_t(GL_DEPTH_STENCIL_OES, GL_UNSIGNED_INT_24_8_OES);
	}
	AssertMsg(0, "Attempted to convert an unsupported format to OpenGL ES enumerations.");
	return GLESImageFormat_t(0, 0);
};

ImageFormat CShaderAPIGLES2::GetNearestSupportedFormat(ImageFormat fmt) const {
	const HardwareCaps_t &caps = g_pHardwareConfig->Caps();
	switch (fmt) {
	case IMAGE_FORMAT_RGBA8888:
	case IMAGE_FORMAT_RGB888:
	case IMAGE_FORMAT_I8:
	case IMAGE_FORMAT_IA88:
	case IMAGE_FORMAT_A8:
	case IMAGE_FORMAT_BGR565:
	case IMAGE_FORMAT_BGRX5551:
	case IMAGE_FORMAT_BGRA4444:
	case IMAGE_FORMAT_BGRA5551:
	case IMAGE_FORMAT_GLES_D16_RB:
		return fmt;
	case IMAGE_FORMAT_ABGR8888:
	case IMAGE_FORMAT_RGB888_BLUESCREEN:
	case IMAGE_FORMAT_BGR888_BLUESCREEN:
	case IMAGE_FORMAT_ARGB8888:
	case IMAGE_FORMAT_BGRX8888:
		return IMAGE_FORMAT_RGBA8888;
	case IMAGE_FORMAT_BGR888:
		return (caps.m_Ext_BGRA ? fmt : IMAGE_FORMAT_RGB888);
	case IMAGE_FORMAT_RGB565:
		return IMAGE_FORMAT_BGR565;
	case IMAGE_FORMAT_P8:
		return IMAGE_FORMAT_I8;
	case IMAGE_FORMAT_BGRA8888:
		return ((caps.m_Ext_BGRA || caps.m_Ext_TextureFormatBGRA8888) ? fmt : IMAGE_FORMAT_RGBA8888);
	case IMAGE_FORMAT_DXT1:
	case IMAGE_FORMAT_DXT3:
	case IMAGE_FORMAT_DXT5:
	case IMAGE_FORMAT_DXT1_ONEBITALPHA:
		return (caps.m_Ext_TextureCompressionS3TC ? fmt : IMAGE_FORMAT_RGBA8888);
	case IMAGE_FORMAT_UV88:
		return (caps.m_GLESVersion >= 300 ? fmt : IMAGE_FORMAT_IA88);
	case IMAGE_FORMAT_UVWQ8888:
	case IMAGE_FORMAT_UVLX8888:
		return (caps.m_GLESVersion >= 300 ? fmt : IMAGE_FORMAT_RGBA8888);
	case IMAGE_FORMAT_RGBA16161616F:
		return (caps.m_Ext_TextureHalfFloat ? fmt : IMAGE_FORMAT_RGBA8888);
	case IMAGE_FORMAT_R32F:
		return (caps.m_Ext_TextureFloat ? fmt : IMAGE_FORMAT_I8);
	case IMAGE_FORMAT_RGB323232F:
	case IMAGE_FORMAT_RGBA32323232F:
		return (caps.m_Ext_TextureFloat ? fmt : IMAGE_FORMAT_RGBA8888);
	// Falling back from a texture to a renderbuffer is worse than falling back to a lower bit depth.
	case IMAGE_FORMAT_GLES_D16_TEX:
		return (caps.m_Ext_DepthTexture ? fmt : IMAGE_FORMAT_GLES_D16_RB);
	case IMAGE_FORMAT_GLES_D16_NONLINEAR_RB:
		return (caps.m_Ext_DepthNonlinear ? fmt : IMAGE_FORMAT_GLES_D16_RB);
	case IMAGE_FORMAT_GLES_D16_NONLINEAR_TEX:
		if (!caps.m_Ext_DepthNonlinear) {
			return GetNearestSupportedFormat(IMAGE_FORMAT_GLES_D16_TEX);
		}
		return (caps.m_Ext_DepthTexture ? fmt : IMAGE_FORMAT_GLES_D16_NONLINEAR_RB);
	case IMAGE_FORMAT_GLES_D24_RB:
		return (caps.m_Ext_Depth24 ? fmt : GetNearestSupportedFormat(IMAGE_FORMAT_GLES_D16_NONLINEAR_RB));
	case IMAGE_FORMAT_GLES_D24_TEX:
		if (!caps.m_Ext_Depth24) {
			return GetNearestSupportedFormat(IMAGE_FORMAT_GLES_D16_NONLINEAR_TEX);
		}
		return (caps.m_Ext_DepthTexture ? fmt : IMAGE_FORMAT_GLES_D24_RB);
	case IMAGE_FORMAT_GLES_DST24_RB:
		return (caps.m_Ext_PackedDepthStencil ? fmt : GetNearestSupportedFormat(IMAGE_FORMAT_GLES_D24_RB));
	case IMAGE_FORMAT_GLES_DST24_TEX:
		if (!caps.m_Ext_PackedDepthStencil) {
			return GetNearestSupportedFormat(IMAGE_FORMAT_GLES_D24_TEX);
		}
		return (caps.m_Ext_DepthTexture ? fmt : IMAGE_FORMAT_GLES_DST24_RB);
	}
	return IMAGE_FORMAT_UNKNOWN;
}

ImageFormat CShaderAPIGLES2::GetNearestRenderTargetFormat(ImageFormat fmt) const {
	return GetNearestSupportedFormat(fmt);
}
