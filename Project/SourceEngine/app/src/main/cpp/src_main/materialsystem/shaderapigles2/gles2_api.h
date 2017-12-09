// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#ifndef SHADERAPI_GLES2_API_H
#define SHADERAPI_GLES2_API_H

#include "gles2_colorformat.h"
#include "shaderapi/ishaderapi.h"
#include "shaderapi/IShaderDevice.h"
#include "shaderapi/ishaderutil.h"
#include "tier1/utllinkedlist.h"
#include "tier1/utlvector.h"

extern IShaderUtil *g_pShaderUtil;

struct Texture_t {
	unsigned int m_GLTexture;
	int m_WrapU, m_WrapV, m_WrapW;
	int m_MagFilter, m_MinFilter;
};

class CShaderAPIGLES2 : public IShaderDevice, public IShaderAPI {
public:
	enum {
		MAX_SAMPLERS = 32 // In GLES 3 (even on Adreno 5xx), 16 in fragment shaders and 16 in vertex shaders. In GLES 2, 8:0 at least.
	};

	CShaderAPIGLES2();

	// Methods of IShaderDevice.

	virtual ImageFormat GetBackBufferFormat() const;
	virtual void GetBackBufferDimensions(int &width, int &height) const;

	virtual int GetCurrentAdapter() const;

	virtual bool IsUsingGraphics() const;

	virtual int StencilBufferBits() const;

	virtual bool IsAAEnabled() const;

	virtual void Present();

	virtual void SetHardwareGammaRamp(float fGamma, float fGammaTVRangeMin, float fGammaTVRangeMax, float fGammaTVExponent, bool bTVEnabled);

	virtual IShaderBuffer *CompileShader(const char *pProgram, size_t nBufLen, const char *pShaderVersion);

	virtual VertexShaderHandle_t CreateVertexShader(IShaderBuffer *pShaderBuffer);
	virtual void DestroyVertexShader(VertexShaderHandle_t hShader);
	virtual GeometryShaderHandle_t CreateGeometryShader(IShaderBuffer *pShaderBuffer);
	virtual void DestroyGeometryShader(GeometryShaderHandle_t hShader);
	virtual PixelShaderHandle_t CreatePixelShader(IShaderBuffer *pShaderBuffer);
	virtual void DestroyPixelShader(PixelShaderHandle_t hShader);

	virtual void EnableNonInteractiveMode(MaterialNonInteractiveMode_t mode, ShaderNonInteractiveInfo_t *pInfo);
	virtual void RefreshFrontBufferNonInteractive();

	// Methods of IShaderAPI.

	virtual void SetViewports(int nCount, const ShaderViewport_t *pViewports);
	virtual int GetViewports(ShaderViewport_t *pViewports, int nMax) const;

	virtual void BindVertexShader(VertexShaderHandle_t hVertexShader);
	virtual void BindGeometryShader(GeometryShaderHandle_t hGeometryShader);
	virtual void BindPixelShader(PixelShaderHandle_t hPixelShader);

	virtual void SetRasterState(const ShaderRasterState_t &state);

	virtual bool SetMode(void *hWnd, int nAdapter, const ShaderDeviceInfo_t &info);

	virtual StateSnapshot_t TakeSnapshot();

	virtual bool IsTranslucent(StateSnapshot_t id) const;
	virtual bool IsAlphaTested(StateSnapshot_t id) const;
	virtual bool UsesVertexAndPixelShaders(StateSnapshot_t id) const;
	virtual bool IsDepthWriteEnabled(StateSnapshot_t id) const;

	// virtual ImageFormat GetNearestSupportedFormat(ImageFormat fmt) const;

	virtual bool DoRenderTargetsNeedSeparateDepthBuffer() const;

	virtual ShaderAPITextureHandle_t CreateTexture(int width, int height, int depth,
			ImageFormat dstImageFormat, int numMipLevels, int numCopies, int flags,
			const char *pDebugName, const char *pTextureGroupName);

	virtual void CreateTextures(ShaderAPITextureHandle_t *pHandles, int count, int width, int height, int depth,
			ImageFormat dstImageFormat, int numMipLevels, int numCopies, int flags,
			const char *pDebugName, const char *pTextureGroupName);

	// Internal methods.

	void OnGLESContextInit(bool restore);
	void OnGLESContextShutdown();

	GLESColorFormat_t ImageFormatToGLES(ImageFormat format) const;

private:
	int m_BackBufferWidth, m_BackBufferHeight;

	ShaderViewport_t m_Viewport;

	unsigned char m_ClearColor[3];
	float m_ClearDepth;
	unsigned int m_ClearStencil;

	CUtlFixedLinkedList<Texture_t> m_Textures;
	CUtlVector<ShaderAPITextureHandle_t> m_TexturesDeleted;
	ShaderAPITextureHandle_t m_TexturesBound[MAX_SAMPLERS];
	Sampler_t m_TextureActive;

	void CreateTextureHandles(ShaderAPITextureHandle_t *handles, int count);
};

extern CShaderAPIGLES2 *g_pShaderAPI;

#endif
