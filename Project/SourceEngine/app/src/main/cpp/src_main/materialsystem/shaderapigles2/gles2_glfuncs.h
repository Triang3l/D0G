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

#ifndef GL_OES_texture_3D
#define GL_OES_texture_3D 1
#define GL_TEXTURE_WRAP_R_OES                                   0x8072
#define GL_TEXTURE_3D_OES                                       0x806F
#define GL_TEXTURE_BINDING_3D_OES                               0x806A
#define GL_MAX_3D_TEXTURE_SIZE_OES                              0x8073
#define GL_SAMPLER_3D_OES                                       0x8B5F
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_OES        0x8CD4
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
	void (GL_APIENTRY *BindAttribLocation)(GLuint program, GLuint index, const GLchar* name);
	void (GL_APIENTRY *BindBuffer)(GLenum target, GLuint buffer);
	void (GL_APIENTRY *BindFramebuffer)(GLenum target, GLuint framebuffer);
	void (GL_APIENTRY *BindRenderbuffer)(GLenum target, GLuint renderbuffer);
	void (GL_APIENTRY *BindTexture)(GLenum target, GLuint texture);
	void (GL_APIENTRY *BlendColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
	void (GL_APIENTRY *BlendEquation)( GLenum mode );
	void (GL_APIENTRY *BlendEquationSeparate)(GLenum modeRGB, GLenum modeAlpha);
	void (GL_APIENTRY *BlendFunc)(GLenum sfactor, GLenum dfactor);
	void (GL_APIENTRY *BlendFuncSeparate)(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
	void (GL_APIENTRY *BufferData)(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
	void (GL_APIENTRY *BufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);
	GLenum (GL_APIENTRY *CheckFramebufferStatus)(GLenum target);
	void (GL_APIENTRY *Clear)(GLbitfield mask);
	void (GL_APIENTRY *ClearColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
	void (GL_APIENTRY *ClearDepthf)(GLclampf depth);
	void (GL_APIENTRY *ClearStencil)(GLint s);
	void (GL_APIENTRY *ColorMask)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
	void (GL_APIENTRY *CompileShader)(GLuint shader);
	void (GL_APIENTRY *CompressedTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid* data);
	void (GL_APIENTRY *CompressedTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid* data);
	void (GL_APIENTRY *CopyTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
	void (GL_APIENTRY *CopyTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
	GLuint (GL_APIENTRY *CreateProgram)(void);
	GLuint (GL_APIENTRY *CreateShader)(GLenum type);
	void (GL_APIENTRY *CullFace)(GLenum mode);
	void (GL_APIENTRY *DeleteBuffers)(GLsizei n, const GLuint* buffers);
	void (GL_APIENTRY *DeleteFramebuffers)(GLsizei n, const GLuint* framebuffers);
	void (GL_APIENTRY *DeleteProgram)(GLuint program);
	void (GL_APIENTRY *DeleteRenderbuffers)(GLsizei n, const GLuint* renderbuffers);
	void (GL_APIENTRY *DeleteShader)(GLuint shader);
	void (GL_APIENTRY *DeleteTextures)(GLsizei n, const GLuint* textures);
	void (GL_APIENTRY *DepthFunc)(GLenum func);
	void (GL_APIENTRY *DepthMask)(GLboolean flag);
	void (GL_APIENTRY *DepthRangef)(GLclampf zNear, GLclampf zFar);
	void (GL_APIENTRY *DetachShader)(GLuint program, GLuint shader);
	void (GL_APIENTRY *Disable)(GLenum cap);
	void (GL_APIENTRY *DisableVertexAttribArray)(GLuint index);
	void (GL_APIENTRY *DrawArrays)(GLenum mode, GLint first, GLsizei count);
	void (GL_APIENTRY *DrawElements)(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
	void (GL_APIENTRY *Enable)(GLenum cap);
	void (GL_APIENTRY *EnableVertexAttribArray)(GLuint index);
	void (GL_APIENTRY *Finish)(void);
	void (GL_APIENTRY *Flush)(void);
	void (GL_APIENTRY *FramebufferRenderbuffer)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
	void (GL_APIENTRY *FramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
	void (GL_APIENTRY *FrontFace)(GLenum mode);
	void (GL_APIENTRY *GenBuffers)(GLsizei n, GLuint* buffers);
	void (GL_APIENTRY *GenerateMipmap)(GLenum target);
	void (GL_APIENTRY *GenFramebuffers)(GLsizei n, GLuint* framebuffers);
	void (GL_APIENTRY *GenRenderbuffers)(GLsizei n, GLuint* renderbuffers);
	void (GL_APIENTRY *GenTextures)(GLsizei n, GLuint* textures);
	void (GL_APIENTRY *GetActiveAttrib)(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
	void (GL_APIENTRY *GetActiveUniform)(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
	void (GL_APIENTRY *GetAttachedShaders)(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders);
	GLint (GL_APIENTRY *GetAttribLocation)(GLuint program, const GLchar* name);
	void (GL_APIENTRY *GetBooleanv)(GLenum pname, GLboolean* params);
	void (GL_APIENTRY *GetBufferParameteriv)(GLenum target, GLenum pname, GLint* params);
	GLenum (GL_APIENTRY *GetError)(void);
	void (GL_APIENTRY *GetFloatv)(GLenum pname, GLfloat* params);
	void (GL_APIENTRY *GetFramebufferAttachmentParameteriv)(GLenum target, GLenum attachment, GLenum pname, GLint* params);
	void (GL_APIENTRY *GetIntegerv)(GLenum pname, GLint* params);
	void (GL_APIENTRY *GetProgramiv)(GLuint program, GLenum pname, GLint* params);
	void (GL_APIENTRY *GetProgramInfoLog)(GLuint program, GLsizei bufsize, GLsizei* length, GLchar* infolog);
	void (GL_APIENTRY *GetRenderbufferParameteriv)(GLenum target, GLenum pname, GLint* params);
	void (GL_APIENTRY *GetShaderiv)(GLuint shader, GLenum pname, GLint* params);
	void (GL_APIENTRY *GetShaderInfoLog)(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* infolog);
	void (GL_APIENTRY *GetShaderPrecisionFormat)(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);
	void (GL_APIENTRY *GetShaderSource)(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* source);
	const GLubyte* (GL_APIENTRY *GetString)(GLenum name);
	void (GL_APIENTRY *GetTexParameterfv)(GLenum target, GLenum pname, GLfloat* params);
	void (GL_APIENTRY *GetTexParameteriv)(GLenum target, GLenum pname, GLint* params);
	void (GL_APIENTRY *GetUniformfv)(GLuint program, GLint location, GLfloat* params);
	void (GL_APIENTRY *GetUniformiv)(GLuint program, GLint location, GLint* params);
	GLint (GL_APIENTRY *GetUniformLocation)(GLuint program, const GLchar* name);
	void (GL_APIENTRY *GetVertexAttribfv)(GLuint index, GLenum pname, GLfloat* params);
	void (GL_APIENTRY *GetVertexAttribiv)(GLuint index, GLenum pname, GLint* params);
	void (GL_APIENTRY *GetVertexAttribPointerv)(GLuint index, GLenum pname, GLvoid** pointer);
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
	void (GL_APIENTRY *ReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels);
	void (GL_APIENTRY *ReleaseShaderCompiler)(void);
	void (GL_APIENTRY *RenderbufferStorage)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
	void (GL_APIENTRY *SampleCoverage)(GLclampf value, GLboolean invert);
	void (GL_APIENTRY *Scissor)(GLint x, GLint y, GLsizei width, GLsizei height);
	void (GL_APIENTRY *ShaderBinary)(GLsizei n, const GLuint* shaders, GLenum binaryformat, const GLvoid* binary, GLsizei length);
	void (GL_APIENTRY *ShaderSource)(GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
	void (GL_APIENTRY *StencilFunc)(GLenum func, GLint ref, GLuint mask);
	void (GL_APIENTRY *StencilFuncSeparate)(GLenum face, GLenum func, GLint ref, GLuint mask);
	void (GL_APIENTRY *StencilMask)(GLuint mask);
	void (GL_APIENTRY *StencilMaskSeparate)(GLenum face, GLuint mask);
	void (GL_APIENTRY *StencilOp)(GLenum fail, GLenum zfail, GLenum zpass);
	void (GL_APIENTRY *StencilOpSeparate)(GLenum face, GLenum fail, GLenum zfail, GLenum zpass);
	void (GL_APIENTRY *TexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels);
	void (GL_APIENTRY *TexParameterf)(GLenum target, GLenum pname, GLfloat param);
	void (GL_APIENTRY *TexParameterfv)(GLenum target, GLenum pname, const GLfloat* params);
	void (GL_APIENTRY *TexParameteri)(GLenum target, GLenum pname, GLint param);
	void (GL_APIENTRY *TexParameteriv)(GLenum target, GLenum pname, const GLint* params);
	void (GL_APIENTRY *TexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels);
	void (GL_APIENTRY *Uniform1f)(GLint location, GLfloat x);
	void (GL_APIENTRY *Uniform1fv)(GLint location, GLsizei count, const GLfloat* v);
	void (GL_APIENTRY *Uniform1i)(GLint location, GLint x);
	void (GL_APIENTRY *Uniform1iv)(GLint location, GLsizei count, const GLint* v);
	void (GL_APIENTRY *Uniform2f)(GLint location, GLfloat x, GLfloat y);
	void (GL_APIENTRY *Uniform2fv)(GLint location, GLsizei count, const GLfloat* v);
	void (GL_APIENTRY *Uniform2i)(GLint location, GLint x, GLint y);
	void (GL_APIENTRY *Uniform2iv)(GLint location, GLsizei count, const GLint* v);
	void (GL_APIENTRY *Uniform3f)(GLint location, GLfloat x, GLfloat y, GLfloat z);
	void (GL_APIENTRY *Uniform3fv)(GLint location, GLsizei count, const GLfloat* v);
	void (GL_APIENTRY *Uniform3i)(GLint location, GLint x, GLint y, GLint z);
	void (GL_APIENTRY *Uniform3iv)(GLint location, GLsizei count, const GLint* v);
	void (GL_APIENTRY *Uniform4f)(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	void (GL_APIENTRY *Uniform4fv)(GLint location, GLsizei count, const GLfloat* v);
	void (GL_APIENTRY *Uniform4i)(GLint location, GLint x, GLint y, GLint z, GLint w);
	void (GL_APIENTRY *Uniform4iv)(GLint location, GLsizei count, const GLint* v);
	void (GL_APIENTRY *UniformMatrix2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
	void (GL_APIENTRY *UniformMatrix3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
	void (GL_APIENTRY *UniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
	void (GL_APIENTRY *UseProgram)(GLuint program);
	void (GL_APIENTRY *ValidateProgram)(GLuint program);
	void (GL_APIENTRY *VertexAttrib1f)(GLuint indx, GLfloat x);
	void (GL_APIENTRY *VertexAttrib1fv)(GLuint indx, const GLfloat* values);
	void (GL_APIENTRY *VertexAttrib2f)(GLuint indx, GLfloat x, GLfloat y);
	void (GL_APIENTRY *VertexAttrib2fv)(GLuint indx, const GLfloat* values);
	void (GL_APIENTRY *VertexAttrib3f)(GLuint indx, GLfloat x, GLfloat y, GLfloat z);
	void (GL_APIENTRY *VertexAttrib3fv)(GLuint indx, const GLfloat* values);
	void (GL_APIENTRY *VertexAttrib4f)(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	void (GL_APIENTRY *VertexAttrib4fv)(GLuint indx, const GLfloat* values);
	void (GL_APIENTRY *VertexAttribPointer)(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr);
	void (GL_APIENTRY *Viewport)(GLint x, GLint y, GLsizei width, GLsizei height);

	// OpenGL ES 3.0 or OES_texture_3D.
	void (GL_APIENTRY *TexImage3D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid* pixels);
	void (GL_APIENTRY *TexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid* pixels);
	void (GL_APIENTRY *CopyTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
	void (GL_APIENTRY *CompressedTexImage3D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid* data);
	void (GL_APIENTRY *CompressedTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid* data);
	void (GL_APIENTRY *FramebufferTexture3D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
};

extern GLES2Funcs_t *g_pGL;

#endif
