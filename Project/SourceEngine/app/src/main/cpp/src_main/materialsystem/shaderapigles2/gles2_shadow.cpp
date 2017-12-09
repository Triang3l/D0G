// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#include "gles2_shadow.h"
#include "gles2_api.h"
#include "tier0/dbg.h"
#include <string.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static CShaderShadowGLES2 s_ShaderShadow;
CShaderShadowGLES2 *g_pShaderShadow = &s_ShaderShadow;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderShadowGLES2, IShaderShadow,
		SHADERSHADOW_INTERFACE_VERSION, s_ShaderShadow)

CShaderShadowGLES2::CShaderShadowGLES2() :
		m_SnapshotList(0, 256), m_SnapshotDict(0, 256) {
	memset(&m_ShadowState, 0, sizeof(m_ShadowState)); // To clear the paddings for CRC.
}

// Snapshot creation.

void CShaderShadowGLES2::SetDefaultState() {
	DepthFunc(SHADER_DEPTHFUNC_NEAREROREQUAL);
	EnableDepthWrites(true);
	EnableDepthTest(true);
	EnablePolyOffset(SHADER_POLYOFFSET_DISABLE);
	EnableColorWrites(true);
	EnableAlphaWrites(false);
	EnableBlending(false);
	BlendFunc(SHADER_BLEND_ONE, SHADER_BLEND_ZERO); // Different from ShaderAPIDX9, but ZERO/ZERO is weird.
	EnableBlendingSeparateAlpha(false);
	BlendFuncSeparateAlpha(SHADER_BLEND_ONE, SHADER_BLEND_ZERO);
	EnableCulling(true);
	EnableAlphaToCoverage(false);
	SetVertexAndPixelShader(NULL, 0, NULL, 0);
}

void CShaderShadowGLES2::DepthFunc(ShaderDepthFunc_t depthFunc) {
	GLenum comparisonFunc;
	switch (depthFunc) {
	case SHADER_DEPTHFUNC_NEVER:
		comparisonFunc = GL_NEVER;
		break;
	case SHADER_DEPTHFUNC_NEARER:
		comparisonFunc = (g_pShaderUtil->GetConfig().bReverseDepth ? GL_GREATER : GL_LESS);
		break;
	case SHADER_DEPTHFUNC_EQUAL:
		comparisonFunc = GL_EQUAL;
		break;
	case SHADER_DEPTHFUNC_NEAREROREQUAL:
		comparisonFunc = (g_pShaderUtil->GetConfig().bReverseDepth ? GL_GEQUAL : GL_LEQUAL);
		break;
	case SHADER_DEPTHFUNC_FARTHER:
		comparisonFunc = (g_pShaderUtil->GetConfig().bReverseDepth ? GL_LESS : GL_GREATER);
		break;
	case SHADER_DEPTHFUNC_NOTEQUAL:
		comparisonFunc = GL_NOTEQUAL;
		break;
	case SHADER_DEPTHFUNC_FARTHEROREQUAL:
		comparisonFunc = (g_pShaderUtil->GetConfig().bReverseDepth ? GL_LEQUAL : GL_GEQUAL);
		break;
	case SHADER_DEPTHFUNC_ALWAYS:
		comparisonFunc = GL_ALWAYS;
		break;
	default SHADER_DEPTHFUNC_ALWAYS:
		comparisonFunc = GL_ALWAYS;
		Warning("DepthFunc: invalid function\n");
		break;
	}
	m_ShadowState.m_DepthFunc = comparisonFunc;
}

void CShaderShadowGLES2::EnableDepthWrites(bool bEnable) {
	m_ShadowState.m_DepthWriteEnable = (bEnable ? 1 : 0);
}

void CShaderShadowGLES2::EnableDepthTest(bool bEnable) {
	m_ShadowState.m_DepthEnable = (bEnable ? 1 : 0);
}

void CShaderShadowGLES2::EnablePolyOffset(PolygonOffsetMode_t nOffsetMode) {
	m_ShadowState.m_DepthBias = (unsigned char) nOffsetMode;
}

void CShaderShadowGLES2::EnableColorWrites(bool bEnable) {
	m_ShadowState.m_ColorWriteEnable = (bEnable ? 1 : 0);
}

void CShaderShadowGLES2::EnableAlphaWrites(bool bEnable) {
	m_ShadowState.m_AlphaWriteEnable = (bEnable ? 1 : 0);
}

GLenum CShaderShadowGLES2::BlendFuncValue(ShaderBlendFactor_t factor) const {
	switch (factor) {
	case SHADER_BLEND_ZERO:
		return GL_ZERO;
	case SHADER_BLEND_ONE:
		return GL_ONE;
	case SHADER_BLEND_DST_COLOR:
		return GL_DST_COLOR;
	case SHADER_BLEND_ONE_MINUS_DST_COLOR:
		return GL_ONE_MINUS_DST_COLOR;
	case SHADER_BLEND_SRC_ALPHA:
		return GL_SRC_ALPHA;
	case SHADER_BLEND_ONE_MINUS_SRC_ALPHA:
		return GL_ONE_MINUS_SRC_ALPHA;
	case SHADER_BLEND_DST_ALPHA:
		return GL_DST_ALPHA;
	case SHADER_BLEND_ONE_MINUS_DST_ALPHA:
		return GL_ONE_MINUS_DST_ALPHA;
	case SHADER_BLEND_SRC_ALPHA_SATURATE:
		return GL_SRC_ALPHA_SATURATE;
	case SHADER_BLEND_SRC_COLOR:
		return GL_SRC_COLOR;
	case SHADER_BLEND_ONE_MINUS_SRC_COLOR:
		return GL_ONE_MINUS_SRC_COLOR;
	}
	Warning("BlendFunc: invalid factor\n");
	return GL_ONE;
}

void CShaderShadowGLES2::EnableBlending(bool bEnable) {
	m_ShadowState.m_BlendEnable = (bEnable ? 1 : 0);
}

void CShaderShadowGLES2::BlendFunc(ShaderBlendFactor_t srcFactor, ShaderBlendFactor_t dstFactor) {
	m_ShadowState.m_BlendSrc = BlendFuncValue(srcFactor);
	m_ShadowState.m_BlendDst = BlendFuncValue(dstFactor);
}

void CShaderShadowGLES2::EnableBlendingSeparateAlpha(bool bEnable) {
	m_ShadowState.m_BlendSeparateEnable = (bEnable ? 1 : 0);
}

void CShaderShadowGLES2::BlendFuncSeparateAlpha(ShaderBlendFactor_t srcFactor, ShaderBlendFactor_t dstFactor) {
	m_ShadowState.m_BlendSrcAlpha = BlendFuncValue(srcFactor);
	m_ShadowState.m_BlendDstAlpha = BlendFuncValue(dstFactor);
}

void CShaderShadowGLES2::EnableAlphaTest(bool bEnable) {
	AssertMsg(0, "Alpha testing must be done in a pixel shader.\n");
}

void CShaderShadowGLES2::AlphaFunc(ShaderAlphaFunc_t alphaFunc, float alphaRef) {
	AssertMsg(0, "Alpha testing must be done in a pixel shader.\n");
}

void CShaderShadowGLES2::EnableCulling(bool bEnable) {
	m_ShadowState.m_CullEnable = (bEnable ? 1 : 0);
}

void CShaderShadowGLES2::SetVertexShader(const char *pFileName, int nStaticVshIndex) {
	AssertMsg(0, "Use SetVertexAndPixelShader to set shadow static shader combos.\n");
}

void CShaderShadowGLES2::SetPixelShader(const char *pFileName, int nStaticVshIndex) {
	AssertMsg(0, "Use SetVertexAndPixelShader to set shadow static shader combo.\n");
}

void CShaderShadowGLES2::SetVertexAndPixelShader(const char *pVshProviderName, int nStaticVshIndex,
		const char *pPshProviderName, int nStaticPshIndex) {
	if (pVshProviderName == NULL || pPshProviderName == NULL) {
		m_ShadowState.m_ShaderProgramStaticCombo = -1;
		return;
	}
	// D0GTODO: Check if this is ever called when GLES context isn't available.
	m_ShadowState.m_ShaderProgramStaticCombo = g_pShaderManager->GetVertexPixelProgramStaticCombo(
			pVshProviderName, nStaticVshIndex, pPshProviderName, nStaticPshIndex);
}

void CShaderShadowGLES2::EnableAlphaToCoverage(bool bEnable) {
	m_ShadowState.m_AlphaToCoverageEnable = (bEnable ? 1 : 0);
}

// Snapshot dictionary.

bool CShaderShadowGLES2::SnapshotDictLessFunc::Less(
		const SnapshotDictEntry_t &src1, const SnapshotDictEntry_t &src2, void *ctx) {
	return src1.m_Checksum < src2.m_Checksum;
}

void CShaderShadowGLES2::ResetSnapshots() {
	m_SnapshotDict.RemoveAll();
	m_SnapshotList.RemoveAll();
}

StateSnapshot_t CShaderShadowGLES2::TakeSnapshot() {
	// Finalizing the state - merging states that are the same
	// (truly the same, without side effects like depth mask affecting clearing).

	if (m_ShadowState.m_BlendEnable) {
		if (m_ShadowState.m_BlendSeparateEnable &&
				m_ShadowState.m_BlendSrc == m_ShadowState.m_BlendSrcAlpha &&
				m_ShadowState.m_BlendDst == m_ShadowState.m_BlendDstAlpha) {
			m_ShadowState.m_BlendSeparateEnable = 0;
		}
	} else {
		m_ShadowState.m_BlendSrc = GL_ONE;
		m_ShadowState.m_BlendDst = GL_ZERO;
		m_ShadowState.m_BlendSeparateEnable = 0;
	}
	if (!m_ShadowState.m_BlendSeparateEnable) {
		m_ShadowState.m_BlendSrcAlpha = m_ShadowState.m_BlendSrc;
		m_ShadowState.m_BlendDstAlpha = m_ShadowState.m_BlendDst;
	}

	// Finding the snapshot with the required state if it has already been added.
	// Assuming the padding has already been zeroed out.

	SnapshotDictEntry_t findEntry;
	CRC32_Init(&findEntry.m_Checksum);
	CRC32_ProcessBuffer(&findEntry.m_Checksum, &m_ShadowState, sizeof(ShadowState_t));
	CRC32_Final(&findEntry.m_Checksum);
	int dictIndex = m_SnapshotDict.FindLessOrEqual(findEntry);
	if (dictIndex >= 0) {
		int dictCount = m_SnapshotDict.Count();
		for (; dictIndex < dictCount; ++dictIndex) {
			const SnapshotDictEntry_t &foundEntry = m_SnapshotDict[dictIndex];
			if (foundEntry.m_Checksum > findEntry.m_Checksum) {
				break;
			}
			StateSnapshot_t foundSnapshot = foundEntry.m_Snapshot;
			if (memcmp(&m_SnapshotList[foundSnapshot], &m_ShadowState, sizeof(ShadowState_t)) == 0) {
				return foundSnapshot;
			}
		}
	}

	// Creating a new snapshot.

	findEntry.newSnapshot = m_SnapshotList.AddToTail();
	memcpy(&m_SnapshotList[findEntry.newSnapshot], &m_ShadowState, sizeof(ShadowState_t));
	m_SnapshotDict.Insert(findEntry);
	return findEntry.newSnapshot;
}
