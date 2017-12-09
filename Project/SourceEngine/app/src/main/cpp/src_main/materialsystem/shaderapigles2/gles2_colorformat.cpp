// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#include "gles2_api.h"
#include "gles2_glfuncs.h"
#include "gles2_hardwareconfig.h"
#include "tier0/dbg.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

GLESImageFormat_t CShaderAPIGLES2::ImageFormatToGLES(ImageFormat format) const {
	// This doesn't care whether it's supported or not.
	const HardwareCaps_t &caps = g_pHardwareConfig->Caps();
	switch (format) {
	case IMAGE_FORMAT_RGBA8888:
		return GLESImageFormat_t(GL_RGBA, GL_UNSIGNED_BYTE);
	// case IMAGE_FORMAT_ABGR8888:
		// Fall back to RGBA8888.
	case IMAGE_FORMAT_RGB888:
	case IMAGE_FORMAT_RGB888_BLUESCREEN:
		return GLESImageFormat_t(GL_RGB, GL_UNSIGNED_BYTE);
	case IMAGE_FORMAT_BGR888:
	case IMAGE_FORMAT_BGR888_BLUESCREEN:
		// GL_EXT_bgra, fall back to RGB888.
		return GLESImageFormat_t(GL_BGR_EXT, GL_UNSIGNED_BYTE);
	// case IMAGE_FORMAT_RGB565:
		// Fall back to BGR565.
	case IMAGE_FORMAT_I8:
		return GLESImageFormat_t(GL_LUMINANCE, GL_UNSIGNED_BYTE);
	case IMAGE_FORMAT_IA88:
		return GLESImageFormat_t(GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE);
	// case IMAGE_FORMAT_P8:
		// Fall back to I8 or A8.
	case IMAGE_FORMAT_A8:
		return GLESImageFormat_t(GL_ALPHA, GL_UNSIGNED_BYTE);
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
	}
	AssertMsg(0, "Attempted to convert an unsupported format to OpenGL ES enumerations.");
	return GLESImageFormat_t(0, 0);
};
