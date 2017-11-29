// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#ifndef SHADERAPI_GLES2_GLFUNCS_H
#define SHADERAPI_GLES2_GLFUNCS_H

#include <GLES2/gl2.h>

#ifndef GL_EXT_bgra
#define GL_EXT_bgra 1
#define GL_BGR_EXT                                              0x80E0
#define GL_BGRA_EXT                                             0x80E1
#endif

#ifndef GL_OES_depth24
#define GL_OES_depth24 1
#define GL_DEPTH_COMPONENT24_OES                                0x81A6
#endif

#ifndef GL_NV_depth_nonlinear
#define GL_NV_depth_nonlinear 1
#define GL_DEPTH_COMPONENT16_NONLINEAR_NV                       0x8E2C
#endif

#ifndef GL_OES_packed_depth_stencil
#define GL_OES_packed_depth_stencil 1
#define GL_DEPTH_STENCIL_OES                                    0x84F9
#define GL_UNSIGNED_INT_24_8_OES                                0x84FA
#define GL_DEPTH24_STENCIL8_OES                                 0x88F0
#endif

#ifndef GL_EXT_shadow_samplers
#define GL_EXT_shadow_samplers 1
#define GL_TEXTURE_COMPARE_MODE_EXT                             0x884C
#define GL_TEXTURE_COMPARE_FUNC_EXT                             0x884D
#define GL_COMPARE_REF_TO_TEXTURE_EXT                           0x884E
#define GL_SAMPLER_2D_SHADOW_EXT                                0x8B62
#endif

#ifndef GL_EXT_texture_compression_s3tc
#define GL_EXT_texture_compression_s3tc 1
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT                         0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT                        0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT                        0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT                        0x83F3
#endif

#ifndef GL_EXT_texture_filter_anisotropic
#define GL_EXT_texture_filter_anisotropic 1
#define GL_TEXTURE_MAX_ANISOTROPY_EXT                           0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT                       0x84FF
#endif

#if !defined(GL_OES_texture_half_float) && !defined(GL_OES_vertex_half_float)
#define GL_OES_texture_half_float 1
#define GL_OES_vertex_half_float 1
#define GL_HALF_FLOAT_OES                                       0x8D61
#endif

#ifndef GL_ES_VERSION_3_0
#define GL_RED                                           0x1903
#define GL_RGBA32F                                       0x8814
#define GL_RGBA16F                                       0x881A
#define GL_HALF_FLOAT                                    0x140B
#define GL_RG                                            0x8227
#define GL_R32F                                          0x822E
#define GL_RG8_SNORM                                     0x8F95
#define GL_RGBA8_SNORM                                   0x8F97
#endif

struct GLES2Funcs_t {
	// OpenGL ES 2.0 core.
	void (GL_APIENTRY *ActiveTexture)(GLenum texture);
	void (GL_APIENTRY *AttachShader)(GLuint program, GLuint shader);
	void (GL_APIENTRY *BindAttribLocation)(GLuint program, GLuint index, const GLchar *name);
	void (GL_APIENTRY *BindBuffer)(GLenum target, GLuint buffer);
	void (GL_APIENTRY *BindFramebuffer)(GLenum target, GLuint framebuffer);
	void (GL_APIENTRY *BindRenderbuffer)(GLenum target, GLuint renderbuffer);
	void (GL_APIENTRY *BindTexture)(GLenum target, GLuint texture);
	void (GL_APIENTRY *BlendColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	void (GL_APIENTRY *BlendEquation)(GLenum mode);
	void (GL_APIENTRY *BlendEquationSeparate)(GLenum modeRGB, GLenum modeAlpha);
	void (GL_APIENTRY *BlendFunc)(GLenum sfactor, GLenum dfactor);
	void (GL_APIENTRY *BlendFuncSeparate)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
	void (GL_APIENTRY *BufferData)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
	void (GL_APIENTRY *BufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
	GLenum (GL_APIENTRY *CheckFramebufferStatus)(GLenum target);
	void (GL_APIENTRY *Clear)(GLbitfield mask);
	void (GL_APIENTRY *ClearColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	void (GL_APIENTRY *ClearDepthf)(GLfloat d);
	void (GL_APIENTRY *ClearStencil)(GLint s);
	void (GL_APIENTRY *ColorMask)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
	void (GL_APIENTRY *CompileShader)(GLuint shader);
	void (GL_APIENTRY *CompressedTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data);
	void (GL_APIENTRY *CompressedTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);
	void (GL_APIENTRY *CopyTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
	void (GL_APIENTRY *CopyTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
	GLuint (GL_APIENTRY *CreateProgram)(void);
	GLuint (GL_APIENTRY *CreateShader)(GLenum type);
	void (GL_APIENTRY *CullFace)(GLenum mode);
	void (GL_APIENTRY *DeleteBuffers)(GLsizei n, const GLuint *buffers);
	void (GL_APIENTRY *DeleteFramebuffers)(GLsizei n, const GLuint *framebuffers);
	void (GL_APIENTRY *DeleteProgram)(GLuint program);
	void (GL_APIENTRY *DeleteRenderbuffers)(GLsizei n, const GLuint *renderbuffers);
	void (GL_APIENTRY *DeleteShader)(GLuint shader);
	void (GL_APIENTRY *DeleteTextures)(GLsizei n, const GLuint *textures);
	void (GL_APIENTRY *DepthFunc)(GLenum func);
	void (GL_APIENTRY *DepthMask)(GLboolean flag);
	void (GL_APIENTRY *DepthRangef)(GLfloat n, GLfloat f);
	void (GL_APIENTRY *DetachShader)(GLuint program, GLuint shader);
	void (GL_APIENTRY *Disable)(GLenum cap);
	void (GL_APIENTRY *DisableVertexAttribArray)(GLuint index);
	void (GL_APIENTRY *DrawArrays)(GLenum mode, GLint first, GLsizei count);
	void (GL_APIENTRY *DrawElements)(GLenum mode, GLsizei count, GLenum type, const void *indices);
	void (GL_APIENTRY *Enable)(GLenum cap);
	void (GL_APIENTRY *EnableVertexAttribArray)(GLuint index);
	void (GL_APIENTRY *Finish)(void);
	void (GL_APIENTRY *Flush)(void);
	void (GL_APIENTRY *FramebufferRenderbuffer)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
	void (GL_APIENTRY *FramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
	void (GL_APIENTRY *FrontFace)(GLenum mode);
	void (GL_APIENTRY *GenBuffers)(GLsizei n, GLuint *buffers);
	void (GL_APIENTRY *GenerateMipmap)(GLenum target);
	void (GL_APIENTRY *GenFramebuffers)(GLsizei n, GLuint *framebuffers);
	void (GL_APIENTRY *GenRenderbuffers)(GLsizei n, GLuint *renderbuffers);
	void (GL_APIENTRY *GenTextures)(GLsizei n, GLuint *textures);
	void (GL_APIENTRY *GetActiveAttrib)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
	void (GL_APIENTRY *GetActiveUniform)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
	void (GL_APIENTRY *GetAttachedShaders)(GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders);
	GLint (GL_APIENTRY *GetAttribLocation)(GLuint program, const GLchar *name);
	void (GL_APIENTRY *GetBooleanv)(GLenum pname, GLboolean *data);
	void (GL_APIENTRY *GetBufferParameteriv)(GLenum target, GLenum pname, GLint *params);
	GLenum (GL_APIENTRY *GetError)(void);
	void (GL_APIENTRY *GetFloatv)(GLenum pname, GLfloat *data);
	void (GL_APIENTRY *GetFramebufferAttachmentParameteriv)(GLenum target, GLenum attachment, GLenum pname, GLint *params);
	void (GL_APIENTRY *GetIntegerv)(GLenum pname, GLint *data);
	void (GL_APIENTRY *GetProgramiv)(GLuint program, GLenum pname, GLint *params);
	void (GL_APIENTRY *GetProgramInfoLog)(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
	void (GL_APIENTRY *GetRenderbufferParameteriv)(GLenum target, GLenum pname, GLint *params);
	void (GL_APIENTRY *GetShaderiv)(GLuint shader, GLenum pname, GLint *params);
	void (GL_APIENTRY *GetShaderInfoLog)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
	void (GL_APIENTRY *GetShaderPrecisionFormat)(GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision);
	void (GL_APIENTRY *GetShaderSource)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source);
	const GLubyte *(GL_APIENTRY *GetString)(GLenum name);
	void (GL_APIENTRY *GetTexParameterfv)(GLenum target, GLenum pname, GLfloat *params);
	void (GL_APIENTRY *GetTexParameteriv)(GLenum target, GLenum pname, GLint *params);
	void (GL_APIENTRY *GetUniformfv)(GLuint program, GLint location, GLfloat *params);
	void (GL_APIENTRY *GetUniformiv)(GLuint program, GLint location, GLint *params);
	GLint (GL_APIENTRY *GetUniformLocation)(GLuint program, const GLchar *name);
	void (GL_APIENTRY *GetVertexAttribfv)(GLuint index, GLenum pname, GLfloat *params);
	void (GL_APIENTRY *GetVertexAttribiv)(GLuint index, GLenum pname, GLint *params);
	void (GL_APIENTRY *GetVertexAttribPointerv)(GLuint index, GLenum pname, void **pointer);
	void (GL_APIENTRY *Hint)(GLenum target, GLenum mode);
	GLboolean (GL_APIENTRY *IsBuffer)(GLuint buffer);
	GLboolean (GL_APIENTRY *IsEnabled)(GLenum cap);
	GLboolean (GL_APIENTRY *IsFramebuffer)(GLuint framebuffer);
	GLboolean (GL_APIENTRY *IsProgram)(GLuint program);
	GLboolean (GL_APIENTRY *IsRenderbuffer)(GLuint renderbuffer);
	GLboolean (GL_APIENTRY *IsShader)(GLuint shader);
	GLboolean (GL_APIENTRY *IsTexture)(GLuint texture);
	void (GL_APIENTRY *LineWidth)(GLfloat width);
	void (GL_APIENTRY *LinkProgram)(GLuint program);
	void (GL_APIENTRY *PixelStorei)(GLenum pname, GLint param);
	void (GL_APIENTRY *PolygonOffset)(GLfloat factor, GLfloat units);
	void (GL_APIENTRY *ReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);
	void (GL_APIENTRY *ReleaseShaderCompiler)(void);
	void (GL_APIENTRY *RenderbufferStorage)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
	void (GL_APIENTRY *SampleCoverage)(GLfloat value, GLboolean invert);
	void (GL_APIENTRY *Scissor)(GLint x, GLint y, GLsizei width, GLsizei height);
	void (GL_APIENTRY *ShaderBinary)(GLsizei count, const GLuint *shaders, GLenum binaryformat, const void *binary, GLsizei length);
	void (GL_APIENTRY *ShaderSource)(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
	void (GL_APIENTRY *StencilFunc)(GLenum func, GLint ref, GLuint mask);
	void (GL_APIENTRY *StencilFuncSeparate)(GLenum face, GLenum func, GLint ref, GLuint mask);
	void (GL_APIENTRY *StencilMask)(GLuint mask);
	void (GL_APIENTRY *StencilMaskSeparate)(GLenum face, GLuint mask);
	void (GL_APIENTRY *StencilOp)(GLenum fail, GLenum zfail, GLenum zpass);
	void (GL_APIENTRY *StencilOpSeparate)(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
	void (GL_APIENTRY *TexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
	void (GL_APIENTRY *TexParameterf)(GLenum target, GLenum pname, GLfloat param);
	void (GL_APIENTRY *TexParameterfv)(GLenum target, GLenum pname, const GLfloat *params);
	void (GL_APIENTRY *TexParameteri)(GLenum target, GLenum pname, GLint param);
	void (GL_APIENTRY *TexParameteriv)(GLenum target, GLenum pname, const GLint *params);
	void (GL_APIENTRY *TexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
	void (GL_APIENTRY *Uniform1f)(GLint location, GLfloat v0);
	void (GL_APIENTRY *Uniform1fv)(GLint location, GLsizei count, const GLfloat *value);
	void (GL_APIENTRY *Uniform1i)(GLint location, GLint v0);
	void (GL_APIENTRY *Uniform1iv)(GLint location, GLsizei count, const GLint *value);
	void (GL_APIENTRY *Uniform2f)(GLint location, GLfloat v0, GLfloat v1);
	void (GL_APIENTRY *Uniform2fv)(GLint location, GLsizei count, const GLfloat *value);
	void (GL_APIENTRY *Uniform2i)(GLint location, GLint v0, GLint v1);
	void (GL_APIENTRY *Uniform2iv)(GLint location, GLsizei count, const GLint *value);
	void (GL_APIENTRY *Uniform3f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
	void (GL_APIENTRY *Uniform3fv)(GLint location, GLsizei count, const GLfloat *value);
	void (GL_APIENTRY *Uniform3i)(GLint location, GLint v0, GLint v1, GLint v2);
	void (GL_APIENTRY *Uniform3iv)(GLint location, GLsizei count, const GLint *value);
	void (GL_APIENTRY *Uniform4f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
	void (GL_APIENTRY *Uniform4fv)(GLint location, GLsizei count, const GLfloat *value);
	void (GL_APIENTRY *Uniform4i)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
	void (GL_APIENTRY *Uniform4iv)(GLint location, GLsizei count, const GLint *value);
	void (GL_APIENTRY *UniformMatrix2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	void (GL_APIENTRY *UniformMatrix3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	void (GL_APIENTRY *UniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	void (GL_APIENTRY *UseProgram)(GLuint program);
	void (GL_APIENTRY *ValidateProgram)(GLuint program);
	void (GL_APIENTRY *VertexAttrib1f)(GLuint index, GLfloat x);
	void (GL_APIENTRY *VertexAttrib1fv)(GLuint index, const GLfloat *v);
	void (GL_APIENTRY *VertexAttrib2f)(GLuint index, GLfloat x, GLfloat y);
	void (GL_APIENTRY *VertexAttrib2fv)(GLuint index, const GLfloat *v);
	void (GL_APIENTRY *VertexAttrib3f)(GLuint index, GLfloat x, GLfloat y, GLfloat z);
	void (GL_APIENTRY *VertexAttrib3fv)(GLuint index, const GLfloat *v);
	void (GL_APIENTRY *VertexAttrib4f)(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	void (GL_APIENTRY *VertexAttrib4fv)(GLuint index, const GLfloat *v);
	void (GL_APIENTRY *VertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
	void (GL_APIENTRY *Viewport)(GLint x, GLint y, GLsizei width, GLsizei height);
};

extern GLES2Funcs_t *g_pGL;

#endif
