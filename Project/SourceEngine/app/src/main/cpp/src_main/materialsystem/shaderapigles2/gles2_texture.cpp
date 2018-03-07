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
