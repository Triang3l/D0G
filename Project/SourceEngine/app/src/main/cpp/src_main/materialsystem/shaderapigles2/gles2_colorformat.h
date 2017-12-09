// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#ifndef SHADERAPI_GLES2_COLORFORMAT_H
#define SHADERAPI_GLES2_COLORFORMAT_H

#include "gles2_glfuncs.h"

struct GLESImageFormat_t {
	GLenum m_Format;
	GLenum m_InternalFormat;
	GLenum m_Type;

	GLESImageFormat_t() {}

	GLESImageFormat_t(GLenum format, GLenum internalFormat, GLenum type) :
			m_Format(format), m_InternalFormat(internalFormat), m_Type(type) {}

	GLESImageFormat_t(GLenum format, GLenum type) :
			m_Format(format), m_InternalFormat(format), m_Type(type) {}
};

#endif
