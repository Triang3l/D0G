// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#include "gles2_api.h"
#include "gles2_devicemgr.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IShaderUtil *g_pShaderUtil;

static CShaderAPIGLES2 s_ShaderAPI;
CShaderAPIGLES2 *g_pShaderAPI = &s_ShaderAPI;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderAPIGLES2, IShaderAPI,
		SHADERAPI_INTERFACE_VERSION, s_ShaderAPI)

CShaderAPIGLES2::CShaderAPIGLES2() :
		m_BackBufferWidth(1), m_BackBufferHeight(1),
		m_TextureActive(SHADER_SAMPLER0) {
	m_Viewport.Init(0, 0, 1, 1, 0.0f, 1.0f);

	for (int sampler = 0; sampler < MAX_SAMPLERS; ++sampler) {
		m_TexturesBound[sampler] = INVALID_SHADERAPI_TEXTURE_HANDLE;
	}
	m_TextureActive = SHADER_SAMPLER0;
}

void CShaderAPIGLES2::SetRasterState(const ShaderRasterState_t &state) {
}
