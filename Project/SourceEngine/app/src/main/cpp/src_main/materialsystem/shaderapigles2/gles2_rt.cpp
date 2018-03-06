// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#include "gles2_api.h"
#include "gles2_devicemgr.h"
#include "gles2_glfuncs.h"
#include "tier0/dbg.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ImageFormat CShaderAPIGLES2::GetBackBufferFormat() const {
	return IMAGE_FORMAT_RGBA8888;
}

void CShaderAPIGLES2::GetBackBufferDimensions(int &width, int &height) const {
	width = m_BackBufferWidth;
	height = m_BackBufferHeight;
}

int CShaderAPIGLES2::StencilBufferBits() const {
	return g_pShaderDeviceMgr->StencilBufferBits();
}

bool CShaderAPIGLES2::IsAAEnabled() const {
	return g_pShaderDeviceMgr->IsAAEnabled();
}

void CShaderAPIGLES2::Present() {
	g_pShaderDeviceMgr->Present();
}

void CShaderAPIGLES2::SetHardwareGammaRamp(float fGamma, float fGammaTVRangeMin, float fGammaTVRangeMax, float fGammaTVExponent, bool bTVEnabled) {
}

void CShaderAPIGLES2::SetViewports(int nCount, const ShaderViewport_t *pViewports) {
	Assert(nCount == 1 && pViewports[0].m_nVersion == SHADER_VIEWPORT_VERSION);
	if (nCount != 1) {
		return;
	}

	if (pViewports[0].m_nTopLeftX != m_Viewport.m_nTopLeftX || pViewports[0].m_nTopLeftY != m_Viewport.m_nTopLeftY ||
			pViewports[0].m_nWidth != m_Viewport.m_nWidth || pViewports[0].m_nHeight != m_Viewport.m_nHeight) {
		m_Viewport.m_nTopLeftX = pViewports[0].m_nTopLeftX;
		m_Viewport.m_nTopLeftY = pViewports[0].m_nTopLeftY;
		m_Viewport.m_nWidth = pViewports[0].m_nWidth;
		m_Viewport.m_nHeight = pViewports[0].m_nHeight;
		g_pGL->Viewport(m_Viewport.m_nTopLeftX, m_Viewport.m_nTopLeftY, m_Viewport.m_nWidth, m_Viewport.m_nHeight);
	}

	if (pViewports[0].m_flMinZ != m_Viewport.m_flMinZ || pViewports[0].m_flMaxZ != m_Viewport.m_flMaxZ) {
		m_Viewport.m_flMinZ = pViewports[0].m_flMinZ;
		m_Viewport.m_flMaxZ = pViewports[0].m_flMaxZ;
		g_pGL->DepthRangef(m_Viewport.m_flMinZ, m_Viewport.m_flMaxZ);
	}
}

int CShaderAPIGLES2::GetViewports(ShaderViewport_t *pViewports, int nMax) const {
	if (pViewports != nullptr && nMax > 0) {
		pViewports[0] = m_Viewport;
	}
	return 1;
}

bool CShaderAPIGLES2::DoRenderTargetsNeedSeparateDepthBuffer() const {
	return true;
}
