// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#ifndef SHADERAPI_GLES2_DEVICEMGR_H
#define SHADERAPI_GLES2_DEVICEMGR_H

#include "gles2_devicemgr.h"
#include <EGL/egl.h>

#ifndef EGL_NV_depth_nonlinear
#define EGL_NV_depth_nonlinear 1
#define EGL_DEPTH_ENCODING_NV 0x30E2
#define EGL_DEPTH_ENCODING_NONE_NV 0
#define EGL_DEPTH_ENCODING_NONLINEAR_NV 0x30E3
#endif

class CShaderDeviceMgrEGL : public CShaderDeviceMgrBase {
public:
	CShaderDeviceMgrEGL();

protected:
	virtual bool InitWindowSystemInterface();
	virtual void ShutdownWindowSystemInterface();

	virtual bool InitGLES2Library();
	virtual void ShutdownGLES2Library();

	virtual void *GetGLES2CoreFunction(const char *name);
	virtual void *GetGLES2ExtensionFunction(const char *name);

	virtual bool CreateInitGLESContext();
	virtual void DestroyInitGLESContext();

	virtual UpdateGLESContextResult_t UpdateGLESContext(const ShaderDeviceInfo_t &info);
	virtual bool UpdateGLESSurface(void *hWnd, unsigned int width, unsigned int height);

private:
	EGLConfig RequestEGLConfig(int colorSize, int depthSize, int depthEncoding, int stencilSize, int minSwapInterval);

	void ShutdownGLES();

	EGLDisplay m_EGLDisplay;

	bool m_EGLExt_DepthNonlinear;

	EGLConfig m_EGLConfig;
	int m_EGLConfigID;
	int m_EGLConfigNativeVisualID;
	bool m_EGLConfigRequest_UseStencil;
	bool m_EGLConfigRequest_WaitForVSync;

	EGLContext m_EGLContext;
	EGLSurface m_EGLPbuffer;
	EGLSurface m_EGLSurface;

	void *m_GLES2Library;
};

#endif
