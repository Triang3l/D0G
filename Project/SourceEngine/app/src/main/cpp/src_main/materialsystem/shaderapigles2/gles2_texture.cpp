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

	if (depth <= 0) {
		depth = 1;
	}

	CreateTextureHandles(pHandles, count);

	
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

void CShaderAPIGLES2::BindTexture(Sampler_t sampler, ShaderAPITextureHandle_t textureHandle) {
	if (!IsUsingGraphics()) {
		return;
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
