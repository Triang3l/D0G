// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#include "gles2_api.h"
#include "gles2_devicemgr.h"
#include "gles2_glfuncs.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

int CShaderAPIGLES2::GetCurrentAdapter() const {
	return 0;
}

bool CShaderAPIGLES2::IsUsingGraphics() const {
	return g_pShaderDeviceMgr->IsGLESContextActive();
}

bool CShaderAPIGLES2::SetMode(void *hWnd, int nAdapter, const ShaderDeviceInfo_t &info) {
	bool wasActive = IsUsingGraphics();
	CShaderDeviceMgrBase::UpdateGLESContextResult_t updateContextResult = g_pShaderDeviceMgr->UpdateGLESContext(info);
	if (updateContextResult == CShaderDeviceMgrBase::UPDATE_GLES_CONTEXT_FAILED) {
		return false;
	}
	if (!g_pShaderDeviceMgr->UpdateGLESSurface(hWnd, info.m_DisplayMode.m_nWidth, info.m_DisplayMode.m_nHeight)) {
		return false;
	}

	if (updateContextResult == CShaderDeviceMgrBase::UPDATE_GLES_CONTEXT_CREATED) {
		OnGLESContextInit(wasActive);
	}
	m_Viewport.m_nTopLeftX = m_Viewport.m_nTopLeftY = 0;
	m_Viewport.m_nWidth = m_BackBufferWidth;
	m_Viewport.m_nHeight = m_BackBufferHeight;
	g_pGL->Viewport(m_Viewport.m_nTopLeftX, m_Viewport.m_nTopLeftY, m_Viewport.m_nWidth, m_Viewport.m_nHeight);

	return true;
}

void CShaderAPIGLES2::OnGLESContextInit(bool restore) {
	g_pGL->DepthRangef(m_Viewport.m_flMinZ, m_Viewport.m_flMaxZ);

	for (int sampler = 0; sampler < MAX_SAMPLERS; ++sampler) {
		TextureBinding_t &binding = m_TexturesBound[sampler];
		binding.m_Handle = INVALID_SHADERAPI_TEXTURE_HANDLE;
		binding.m_Copy = 0;
	}
	m_TextureActive = SHADER_SAMPLER0;
}

void CShaderAPIGLES2::OnGLESContextShutdown() {
	if (!IsUsingGraphics()) {
		return;
	}
}

void CShaderAPIGLES2::EnableNonInteractiveMode(MaterialNonInteractiveMode_t mode, ShaderNonInteractiveInfo_t *pInfo) {
}

void CShaderAPIGLES2::RefreshFrontBufferNonInteractive() {
}
