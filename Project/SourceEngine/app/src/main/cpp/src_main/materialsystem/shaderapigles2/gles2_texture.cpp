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
	if (m_TexturesBound[sampler] == textureHandle) {
		return;
	}
	GLenum textureTarget = GL_TEXTURE_2D;
	GLuint textureGLName = 0;
	if (textureHandle != INVALID_SHADERAPI_TEXTURE_HANDLE) {
		Texture_t &texture = m_Textures[textureHandle];
		textureTarget = texture.m_Target;
		Assert(textureTarget != GL_RENDERBUFFER);
		if (textureTarget == GL_RENDERBUFFER) {
			return;
		}
		textureGLName = texture.m_GLTexture;
	}
	if (m_TextureActive != sampler) {
		m_TextureActive = sampler;
		g_pGL->ActiveTexture(GL_TEXTURE0 + sampler);
	}
	m_TexturesBound[sampler] = textureHandle;
	g_pGL->BindTexture(textureTarget, textureGLName);
}

void CShaderAPIGLES2::BindVertexTexture(VertexTextureSampler_t nSampler, ShaderAPITextureHandle_t textureHandle) {
	return BindTexture(nSampler);
}
