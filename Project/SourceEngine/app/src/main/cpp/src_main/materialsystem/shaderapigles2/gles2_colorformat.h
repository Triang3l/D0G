// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

struct GLESImageFormat_t {
	unsigned int m_Format;
	unsigned int m_InternalFormat;
	unsigned int m_Type;

	GLESImageFormat_t() {}

	GLESImageFormat_t(unsigned int format, unsigned int internalFormat, unsigned int type) :
			m_Format(format), m_InternalFormat(internalFormat), m_Type(type) {}

	GLESImageFormat_t(unsigned int format, unsigned int type) :
			m_Format(format), m_InternalFormat(format), m_Type(type) {}
};
