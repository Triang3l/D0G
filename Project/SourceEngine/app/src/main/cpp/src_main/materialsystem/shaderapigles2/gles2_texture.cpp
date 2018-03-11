// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#include "gles2_api.h"
#include "gles2_glfuncs.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ShaderAPITextureHandle_t CShaderAPIGLES2::CreateTexture(int width, int height, int depth,
		ImageFormat dstImageFormat, int numMipLevels, int numCopies, int flags,
		const char *pDebugName, const char *pTextureGroupName) {
	ShaderAPITextureHandle_t handle = 0;
	CreateTextures(&handle, 1, width, height, depth, dstImageFormat, numMipLevels, numCopies, flags, pDebugName, pTextureGroupName);
	return handle;
}

void CShaderAPIGLES2::CreateTextures(ShaderAPITextureHandle_t *pHandles,
		int count, int width, int height, int depth,
		ImageFormat dstImageFormat, int numMipLevels, int numCopies, int flags,
		const char *pDebugName, const char *pTextureGroupName) {
	if (count <= 0) {
		return;
	}

	width = MAX(width, 1);
	height = MAX(height, 1);
	depth = MAX(depth, 1);
	numCopies = MAX(numCopies, 1);

	CreateTextureHandles(pHandles, count);

	const GLESImageFormat_t &textureGLESFormat = ImageFormatToGLES(dstImageFormat);
	Assert(textureGLESFormat.m_Format != 0 && textureGLESFormat.m_InternalFormat != 0);
	if (textureGLESFormat.m_Format == 0 || textureGLESFormat.m_InternalFormat == 0) {
		dstImageFormat = IMAGE_FORMAT_UNKNOWN;
	}
	Assert(dstImageFormat != IMAGE_FORMAT_UNKNOWN);
	if (dstImageFormat == IMAGE_FORMAT_UNKNOWN) {
		Warning("ShaderAPIGLES2::CreateTextures: Invalid color format!\n");
	}

	GLenum target, faceTarget;
	int faceCount = 1;
	int maxSize;
	if (IsImageFormatRenderbuffer(dstImageFormat)) {
		target = faceTarget = GL_RENDERBUFFER;
		depth = 1;
		numMipLevels = 1;
		// On most known devices, the maximum renderbuffer size is the same as the maximum 2D texture size.
		// The only exception is Tegra 3, where it's 3839, but the maximum texture size is 2048.
		// Clamping to the same value makes sure depth buffers will always be compatible with color buffers.
		maxSize = g_pHardwareConfig->MaxTextureWidth();
	} else if (flags & TEXTURE_CREATE_CUBEMAP) {
		target = GL_TEXTURE_CUBE_MAP;
		faceTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
		faceCount = 6;
		depth = 1;
		maxSize = g_pHardwareConfig->MaxCubemapSize();
	} else if (depth > 1) {
		target = faceTarget = GL_TEXTURE_3D_OES;
		maxSize = g_pHardwareConfig->MaxTextureDepth();
	} else {
		target = faceTarget = GL_TEXTURE_2D;
		maxSize = g_pHardwareConfig->MaxTextureWidth();
	}
	Assert(width <= maxSize && height <= maxSize && depth <= maxSize);
	width = MIN(width, maxSize);
	height = MIN(height, maxSize);
	depth = MIN(depth, maxSize);
	bool limitedNPOT = (!g_pHardwareConfig->SupportsNonPow2Textures() && (
			((width & (width - 1)) != 0) || ((height & (height - 1)) != 0) || ((depth & (depth - 1)) != 0)));
	if (limitedNPOT) {
		numMipLevels = 1;
	}

	// OpenGL ES 2.0 requires a full pyramid. 0 levels also means creating the full pyramid.
	if (numMipLevels != 1) {
		int mipWidth = width, mipHeight = height, mipDepth = depth;
		numMipLevels = 1;
		while (mipWidth > 1 || mipHeight > 1 || mipDepth > 1) {
			++numMipLevels;
			mipWidth = MAX(mipWidth >> 1, 1);
			mipHeight = MAX(mipHeight >> 1, 1);
			mipDepth = MAX(mipDepth >> 1, 1);
		}
	}

	GLenum filterMin = GL_NEAREST, filterMag = GL_NEAREST;
	// Make depth textures point sampled by default, explicitly enable filtering and comparison when needed.
	if (target != GL_RENDERBUFFER && dstImageFormat != IMAGE_FORMAT_GLES_D16_TEX &&
			dstImageFormat != IMAGE_FORMAT_GLES_D16_NONLINEAR_TEX &&
			dstImageFormat != IMAGE_FORMAT_GLES_D24_TEX && dstImageFormat != IMAGE_FORMAT_GLES_DST24_TEX) {
		filterMin = (numMipLevels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		filterMag = GL_LINEAR;
	}

	int textureGLCount = count * numCopies;
	GLuint *textureGLNames = reinterpret_cast<GLuint *>(stackalloc(textureGLCount * sizeof(GLuint)));
	memset(textureGLNames, 0, textureGLCount * sizeof(GLuint));
	if (dstImageFormat != IMAGE_FORMAT_UNKNOWN) {
		if (target == GL_RENDERBUFFER) {
			g_pGL->CreateRenderbuffers(textureGLCount, textureGLNames);
		} else {
			g_pGL->CreateTextures(target, textureGLCount, textureGLNames);
		}
	}

	for (int frameIndex = 0; frameIndex < count; ++frameIndex) {
		ShaderAPITextureHandle_t handle = pHandles[frameIndex];
		Texture_t &texture = m_Textures[handle];
		texture.m_Target = target;
		texture.m_CopyCount = numCopies;
		if (numCopies > 1) {
			texture.m_GLTextureNames = new unsigned int[numCopies];
			memcpy(texture.m_GLTextureNames, &textureGLNames[frameIndex * numCopies], numCopies * sizeof(unsigned int));
		} else {
			texture.m_GLTextureName = textureGLNames[frameIndex];
		}
		texture.m_Width = width;
		texture.m_Height = height;
		texture.m_Depth = depth;
		texture.m_MipLevels = numMipLevels;
		texture.m_WrapS = texture.m_WrapT = texture.m_WrapR = GL_CLAMP_TO_EDGE;
		texture.m_FilterMin = filterMin;
		texture.m_FilterMag = filterMag;
		for (texture.m_CopyCurrent = 0; texture.m_CopyCurrent < numCopies; ++texture.m_CopyCurrent) {
			GLuint textureGLName = texture.GetGLTexture();
			if (textureGLName == 0) {
				continue;
			}
			if (target == GL_RENDERBUFFER) {
				g_pGL->BindRenderbuffer(GL_RENDERBUFFER, texture.m_GLTextureName);
				g_pGL->RenderbufferStorage(GL_RENDERBUFFER, textureGLESFormat.m_InternalFormat, width, height);
			} else {
				BindTexture(m_TextureActive, handle);
				if (textureGLESFormat.m_Type != 0) { // CompressedTexImage doesn't allow null data.
					if (target == GL_TEXTURE_3D_OES) {
						for (int mipLevel = 0; mipLevel < mipLevels; ++mipLevel) {
							g_pGL->TexImage3D(faceTarget, mipLevel, textureGLESFormat.m_InternalFormat,
									MAX(width >> mipLevel, 1), MAX(height >> mipLevel, 1), MAX(depth >> mipLevel, 1),
									0, textureGLESFormat.m_Format, textureGLESFormat.m_Type, nullptr);
						}
					} else {
						for (int faceIndex = 0; faceIndex < faceCount; ++faceIndex) {
							for (int mipLevel = 0; mipLevel < mipLevels; ++mipLevel) {
								g_pGL->TexImage2D(faceTarget + faceIndex, mipLevel, textureGLESFormat.m_InternalFormat,
										MAX(width >> mipLevel, 1), MAX(height >> mipLevel, 1),
										0, textureGLESFormat.m_Format, textureGLESFormat.m_Type, nullptr);
							}
						}
					}
				}
				g_pGL->TexParameteri(target, GL_TEXTURE_WRAP_S, texture.m_WrapS);
				g_pGL->TexParameteri(target, GL_TEXTURE_WRAP_T, texture.m_WrapT);
				if (target == GL_TEXTURE_3D_OES) {
					g_pGL->TexParameteri(target, GL_TEXTURE_WRAP_R_OES, texture.m_WrapR);
				}
				g_pGL->TexParameteri(target, GL_TEXTURE_MIN_FILTER, texturem.m_FilterMin);
				g_pGL->TexParameteri(target, GL_TEXTURE_MAG_FILTER, texturem.m_FilterMag);
			}
		}
		texture.m_CopyCurrent = 0;
	}
}

void CShaderAPIGLES2::CreateTextureHandles(ShaderAPITextureHandle_t *handles, int count) {
	if (count <= 0) {
		return;
	}

	// D0GTODO: MEM_ALLOC_CREDIT.

	while (count > 0 && m_TexturesDeleted.Count() > 0) {
		*(handles++) = m_TexturesDeleted.Tail();
		m_TexturesDeleted.Remove(m_TexturesDeleted.Count() - 1);
		--count;
	}

	while (count > 0) {
		*(handles++) = m_Textures.AddToTail();
		--count;
	}
}

bool CShaderAPIGLES2::IsTexture(ShaderAPITextureHandle_t textureHandle) {
	return (textureHandle >= 0 && textureHandle < m_Textures.Count() &&
			m_Textures[textureHandle].m_Target != 0);
}

void CShaderAPIGLES2::BindTexture(Sampler_t sampler, ShaderAPITextureHandle_t textureHandle) {
	if (!IsUsingGraphics()) {
		return;
	}
	if (!IsTexture(textureHandle)) {
		textureHandle = INVALID_SHADERAPI_TEXTURE_HANDLE;
	}
	TextureBinding_t &binding = m_TexturesBound[sampler];
	if (textureHandle == INVALID_SHADERAPI_TEXTURE_HANDLE) {
		if (binding.m_Handle == INVALID_SHADERAPI_TEXTURE_HANDLE) {
			return;
		}
		if (m_TextureActive != sampler) {
			g_pGL->ActiveTexture(GL_TEXTURE0 + sampler);
			m_TextureActive = sampler;
		}
		g_pGL->BindTexture(GL_TEXTURE_2D, 0);
		binding.m_Handle = INVALID_SHADERAPI_TEXTURE_HANDLE;
		binding.m_Copy = 0;
	} else {
		const Texture_t &texture = m_Textures[textureHandle];
		if (texture.m_Target == GL_RENDERBUFFER) {
			// Calling texture-related functions on renderbuffers doesn't seem dangerous.
			return;
		}
		unsigned int copy = texture.m_CopyCurrent;
		if (binding.m_Handle == textureHandle && binding.m_Copy == copy) {
			return;
		}
		if (m_TextureActive != sampler) {
			g_pGL->ActiveTexture(GL_TEXTURE0 + sampler);
			m_TextureActive = sampler;
		}
		g_pGL->BindTexture(texture.m_Target, texture.GetGLTexture());
		binding.m_Handle = textureHandle;
		binding.m_Copy = copy;
	}
}

void CShaderAPIGLES2::BindVertexTexture(VertexTextureSampler_t nSampler, ShaderAPITextureHandle_t textureHandle) {
	return BindTexture(nSampler, textureHandle);
}
