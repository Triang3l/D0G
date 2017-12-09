// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#ifndef SHADERAPI_GLES2_SHADOW_H
#define SHADERAPI_GLES2_SHADOW_H

#include "gles2_glfuncs.h"
#include "gles2_shader.h"
#include "shaderapi/ishadershadow.h"
#include "tier1/checksum_crc.h"
#include "tier1/utllinkedlist.h"
#include "tier1/UtlSortVector.h"

struct ShadowState_t {
	ShaderProgramStaticComboHandle_t m_ShaderProgramStaticCombo;

	GLenum m_DepthFunc;

	GLenum m_BlendSrc;
	GLenum m_BlendDst;
	GLenum m_BlendSrcAlpha;
	GLenum m_BlendDstAlpha;

	unsigned char m_CullEnable : 1;

	unsigned char m_DepthEnable : 1;
	unsigned char m_DepthBias : 2;
	unsigned char m_DepthWriteEnable : 1;

	unsigned char m_AlphaToCoverageEnable : 1;

	unsigned char m_BlendEnable : 1;
	unsigned char m_BlendSeparateEnable : 1; // Doesn't matter in the current state.

	unsigned char m_ColorWriteEnable : 1;
	unsigned char m_AlphaWriteEnable : 1;
};

class CShaderShadowGLES2 : public IShaderShadow {
public:
	CShaderShadowGLES2();

	// IShaderShadow methods.

	virtual void SetDefaultState();

	virtual void DepthFunc(ShaderDepthFunc_t depthFunc);
	virtual void EnableDepthWrites(bool bEnable);
	virtual void EnableDepthTest(bool bEnable);
	virtual void EnablePolyOffset(PolygonOffsetMode_t nOffsetMode);

	// These methods for controlling stencil are obsolete and stubbed to do nothing.
	virtual void EnableStencil(bool bEnable) {}
	virtual void StencilFunc(ShaderStencilFunc_t stencilFunc) {}
	virtual void StencilPassOp(ShaderStencilOp_t stencilOp) {}
	virtual void StencilFailOp(ShaderStencilOp_t stencilOp) {}
	virtual void StencilDepthFailOp(ShaderStencilOp_t stencilOp) {}
	virtual void StencilReference(int nReference) {}
	virtual void StencilMask(int nMask) {}
	virtual void StencilWriteMask(int nMask) {}

	virtual void EnableColorWrites(bool bEnable);
	virtual void EnableAlphaWrites(bool bEnable);

	virtual void EnableBlending(bool bEnable);
	virtual void BlendFunc(ShaderBlendFactor_t srcFactor, ShaderBlendFactor_t dstFactor);
	virtual void EnableBlendingSeparateAlpha(bool bEnable);
	virtual void BlendFuncSeparateAlpha(ShaderBlendFactor_t srcFactor, ShaderBlendFactor_t dstFactor);

	virtual void EnableAlphaTest(bool bEnable);
	virtual void AlphaFunc(ShaderAlphaFunc_t alphaFunc, float alphaRef);

	virtual void PolyMode(ShaderPolyModeFace_t face, ShaderPolyMode_t polyMode) {}

	virtual void EnableCulling(bool bEnable);

	virtual void EnableConstantColor(bool bEnable) {}

	// Binding everything the mesh has to all used attributes.
	virtual void VertexShaderVertexFormat(unsigned int nFlags,
			int nTexCoordCount, int *pTexCoordDimensions, int nUserDataSize) {}

	virtual void SetVertexShader(const char *pFileName, int nStaticVshIndex);
	virtual	void SetPixelShader(const char *pFileName, int nStaticPshIndex);
	virtual void SetVertexAndPixelShader(const char *pVshProviderName, int nStaticVshIndex,
			const char *pPshProviderName, int nStaticPshIndex);

	virtual void EnableLighting(bool bEnable) {}
	virtual void EnableSpecular(bool bEnable) {}

	// Requires an extension, too messy, just fix in the shader if really needed.
	virtual void EnableSRGBWrite(bool bEnable) {}

	// Texture format property, not sampler state. Convert in the shaders if really needed.
	virtual void EnableSRGBRead(Sampler_t sampler, bool bEnable) {}

	virtual void EnableVertexBlend(bool bEnable) {}

	virtual void OverbrightValue(TextureStage_t stage, float value) {}
	virtual void EnableTexture(Sampler_t sampler, bool bEnable) {}
	virtual void EnableTexGen(TextureStage_t stage, bool bEnable) {}
	virtual void TexGen(TextureStage_t stage, ShaderTexGenParam_t param) {}

	virtual void EnableCustomPixelPipe(bool bEnable) {}
	virtual void CustomTextureStages(int stageCount) {}
	virtual void CustomTextureOperation(TextureStage_t stage, ShaderTexChannel_t channel,
			ShaderTexOp_t op, ShaderTexArg_t arg1, ShaderTexArg_t arg2) {}

	virtual void DrawFlags(unsigned int drawFlags) {}

	virtual void EnableAlphaPipe(bool bEnable) {}
	virtual void EnableConstantAlpha(bool bEnable) {}
	virtual void EnableVertexAlpha(bool bEnable) {}
	virtual void EnableTextureAlpha(TextureStage_t stage, bool bEnable) {}

	// Entirely dynamic now as there's no fixed-function fog.
	virtual void FogMode(ShaderFogMode_t fogMode) {}

	virtual void SetDiffuseMaterialSource(ShaderMaterialSource_t materialSource) {}

	// Vertex format-related.
	virtual void SetMorphFormat(MorphFormat_t flags) {}

	virtual void DisableFogGammaCorrection(bool bDisable) {}

	virtual void EnableAlphaToCoverage(bool bEnable);

	// Fetch4 is a shader function, not sampler state.
	virtual void SetShadowDepthFiltering(Sampler_t stage) {}

	// Internal methods.

	void ResetSnapshots();

	StateSnapshot_t TakeSnapshot();

private:
	// Snapshot creation.

	GLenum BlendFuncValue(ShaderBlendFactor_t factor) const;

	ShadowState_t m_ShadowState;

	// Snapshot dictionary.

	CUtlFixedLinkedList<ShadowState_t> m_SnapshotList;

	struct SnapshotDictEntry_t {
		CRC32_t m_Checksum;
		StateSnapshot_t m_Snapshot;
	};

	class SnapshotDictLessFunc {
	public:
		bool Less(const SnapshotDictEntry_t &src1, const SnapshotDictEntry_t &src2, void *ctx);
	};

	CUtlSortVector<SnapshotDictEntry_t, SnapshotDictLessFunc> m_SnapshotDict;

	// Current GLES state.

	ShadowState_t m_CurrentState;
};

extern CShaderShadowGLES2 *g_pShaderShadow;

#endif
