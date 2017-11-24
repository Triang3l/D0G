// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#ifndef SHADERAPI_GLES2_DEVICEMGR_H
#define SHADERAPI_GLES2_DEVICEMGR_H

#include "shaderapi/IShaderDevice.h"

class CShaderDeviceMgrBase : public IShaderDeviceMgr {
public:
	// Methods of IAppSystem.
	virtual bool Connect(CreateInterfaceFn factory);
	virtual void Disconnect();
	virtual InitReturnVal_t Init();
	virtual void Shutdown();

	// Methods of IShaderDevice.
	virtual int GetAdapterCount() const;
	virtual void GetAdapterInfo(int nAdapter, MaterialAdapterInfo_t &info) const;
	virtual bool GetRecommendedConfigurationInfo(int nAdapter, int nDXLevel, KeyValues *pConfiguration);
	virtual CreateInterfaceFn SetMode(void *hWnd, int nAdapter, const ShaderDeviceInfo_t &mode);
	virtual int GetModeCount(int nAdapter) const;
	virtual void GetModeInfo(ShaderDisplayMode_t *pInfo, int nAdapter, int nMode) const;
	virtual void GetCurrentModeInfo(ShaderDisplayMode_t *pInfo, int nAdapter) const;

	// Factory to return from SetMode.
	static void *ShaderInterfaceFactory(const char *pInterfaceName, int *pReturnCode);

	enum UpdateGLESContextResult_t {
		UPDATE_GLES_CONTEXT_FAILED,
		UPDATE_GLES_CONTEXT_KEPT, // The old context can be used with the new shader device info.
		UPDATE_GLES_CONTEXT_CREATED // A new context was created and everything needs to be restored.
	};
	// Should be followed by UpdateGLESSurface.
	virtual UpdateGLESContextResult_t UpdateGLESContext(const ShaderDeviceInfo_t &info) = 0;
	// NULL hWnd can be used to make GL calls without drawing.
	virtual bool UpdateGLESSurface(void *hWnd, unsigned int width, unsigned int height) = 0; 

protected:
	CShaderDeviceMgrBase();

	virtual bool InitWindowSystemInterface() = 0;
	virtual void ShutdownWindowSystemInterface() = 0;

	virtual bool InitGLES2Library() = 0;
	virtual void ShutdownGLES2Library() = 0;

	// Temporary context to obtain adapter and extension information from.
	virtual bool CreateInitGLESContext() = 0;
	virtual void DestroyInitGLESContext() = 0;

	virtual void *GetGLES2CoreFunction(const char *name) = 0;
	virtual void *GetGLES2ExtensionFunction(const char *name) = 0;

private:
	bool InitGLES2CoreFunctions();
	void InitGLES2ExtensionsAndGLES3Core();
};

extern CShaderDeviceMgrBase *g_pShaderDeviceMgr;

#endif
