// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#include "gles2_devicemgr_egl.h"
#include "gles2_api.h"
#include "gles2_hardwareconfig.h"
#include "tier0/dbg.h"
#include <string.h>
#ifdef __ANDROID__
#include <dlfcn.h>
#include <android/native_window.h>
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CShaderDeviceMgrEGL::CShaderDeviceMgrEGL() :
		m_EGLDisplay(EGL_NO_DISPLAY),
		m_EGLConfig(nullptr),
		m_EGLConfigID(0),
		m_EGLConfigStencilSize(0),
		m_EGLConfigSamples(1),
		m_EGLContext(EGL_NO_CONTEXT),
		m_EGLPbuffer(EGL_NO_SURFACE),
		m_EGLSurfaceWindow((EGLNativeWindowType) nullptr),
		m_EGLSurface(EGL_NO_SURFACE),
		m_EGLContextActive(false),
		m_GLES2Library(nullptr) {}

bool CShaderDeviceMgrEGL::InitWindowSystemInterface() {
	m_EGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (m_EGLDisplay == EGL_NO_DISPLAY) {
		Warning("EGL ERROR: Failed to get the default EGL display.\n");
		return false;
	}

	if (!eglInitialize(m_EGLDisplay, nullptr, nullptr)) {
		Warning("EGL ERROR: Failed to initialize EGL.\n");
		m_EGLDisplay = EGL_NO_DISPLAY;
		return false;
	}

	const char *extensions = eglQueryString(m_EGLDisplay, EGL_EXTENSIONS);
	m_EGLExt_DepthNonlinear = CHardwareConfig::CheckGLExtension(extensions, "EGL_NV_depth_nonlinear");

	return true;
}

void CShaderDeviceMgrEGL::ShutdownWindowSystemInterface() {
	if (m_EGLDisplay != EGL_NO_DISPLAY) {
		eglTerminate(m_EGLDisplay);
		m_EGLDisplay = EGL_NO_DISPLAY;
	}
}

bool CShaderDeviceMgrEGL::InitGLES2Library() {
#ifdef __ANDROID__
	m_GLES2Library = dlopen("libGLESv2.so", RTLD_LAZY | RTLD_GLOBAL);
	return (m_GLES2Library != nullptr);
#else
#error Implement CShaderDeviceMgrEGL::InitGLES2Library for this platform.
#endif
}

void CShaderDeviceMgrEGL::ShutdownGLES2Library() {
#ifdef __ANDROID__
	if (m_GLES2Library != nullptr) {
		dlclose(m_GLES2Library);
		m_GLES2Library = nullptr;
	}
#else
#error Implement CShaderDeviceMgrEGL::ShutdownGLES2Library for this platform.
#endif
}

void *CShaderDeviceMgrEGL::GetGLES2CoreFunction(const char *name) {
#ifdef __ANDROID__
	return dlsym(m_GLES2Library, name);
#else
#error Implement CShaderDeviceMgrEGL::GetGLES2CoreFunction for this platform.
#endif
}

void *CShaderDeviceMgrEGL::GetGLES2ExtensionFunction(const char *name) {
	return (void *) eglGetProcAddress(name);
}

bool CShaderDeviceMgrEGL::CreateInitGLESContext() {
	int configAttribs[] = {
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
		EGL_NONE
	};
	EGLConfig config = nullptr;
	int configCount = 0;
	EGLBoolean configChosen = eglChooseConfig(m_EGLDisplay, configAttribs, &config, 1, &configCount);
	if (!configChosen || configCount < 0 || config == nullptr) {
		Warning("EGL ERROR: Failed to choose a config for the temporary OpenGL ES context.\n");
		return false;
	}

	const int pbufferAttribs[] = { EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE };
	m_EGLPbuffer = eglCreatePbufferSurface(m_EGLDisplay, config, pbufferAttribs);
	if (m_EGLPbuffer == EGL_NO_SURFACE) {
		Warning("EGL ERROR: Failed to create a 1x1 pbuffer surface for the temporary OpenGL ES context.\n");
		return false;
	}

	const int contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	m_EGLContext = eglCreateContext(m_EGLDisplay, config, EGL_NO_CONTEXT, contextAttribs);
	if (m_EGLContext == EGL_NO_CONTEXT) {
		Warning("EGL ERROR: Failed to create the temporary OpenGL ES context.\n");
		eglDestroySurface(m_EGLDisplay, m_EGLPbuffer);
		m_EGLPbuffer = EGL_NO_SURFACE;
		return false;
	}

	eglMakeCurrent(m_EGLDisplay, m_EGLSurface, m_EGLSurface, m_EGLContext);
	m_EGLContextActive = true;
	return true;
}

void CShaderDeviceMgrEGL::DestroyInitGLESContext() {
	if (m_EGLContext != EGL_NO_CONTEXT) {
		m_EGLContextActive = false;
		eglMakeCurrent(m_EGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		eglDestroyContext(m_EGLDisplay, m_EGLContext);
		m_EGLContext = EGL_NO_CONTEXT;
	}
	if (m_EGLPbuffer != EGL_NO_SURFACE) {
		eglDestroySurface(m_EGLDisplay, m_EGLPbuffer);
		m_EGLPbuffer = EGL_NO_SURFACE;
	}
}

EGLConfig CShaderDeviceMgrEGL::RequestEGLConfig(int depthSize, int depthEncoding, int stencilSize, int minSwapInterval) {
	int attribs[] = {
		/*  0 */ EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		/*  2 */ EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
		/*  4 */ EGL_RED_SIZE, 8,
		/*  6 */ EGL_GREEN_SIZE, 8,
		/*  8 */ EGL_BLUE_SIZE, 8,
		/* 10 */ EGL_ALPHA_SIZE, 8,
		/* 12 */ EGL_DEPTH_SIZE, depthSize,
		/* 14 */ EGL_STENCIL_SIZE, stencilSize,
		/* 16 */ EGL_MIN_SWAP_INTERVAL, minSwapInterval,
		/* 18 */ EGL_NONE, EGL_DONT_CARE,
		EGL_NONE
	};
	unsigned int attribCount = 18;

	if (depthEncoding != EGL_DONT_CARE) {
		attribs[attribCount++] = EGL_DEPTH_ENCODING_NV;
		attribs[attribCount++] = depthEncoding;
	}

	EGLConfig config = nullptr;
	int configCount = 0;
	if (!eglChooseConfig(m_EGLDisplay, attribs, &config, 1, &configCount)) {
		return nullptr;
	}
	if (configCount < 1) {
		return nullptr;
	}
	return config;
}

UpdateGLESContextResult_t CShaderDeviceMgrEGL::UpdateGLESContext(const ShaderDeviceInfo_t &info) {
	bool requestConfig;
	if (m_EGLConfigID != 0) {
		requestConfig = (info.m_bUseStencil != m_EGLConfigRequest_UseStencil ||
				info.m_bWaitForVSync != m_EGLConfigRequest_WaitForVSync);
	} else {
		requestConfig = true;
	}

	EGLConfig config;
	int configID;
	int stencilSize = m_EGLConfigStencilSize;
	int samples = m_EGLConfigSamples;
	if (requestConfig) {
		config = nullptr;
		const int depthSizes[] = { 24, 16, 16 };
		const int depthEncodings[] = { EGL_DONT_CARE, EGL_DEPTH_ENCODING_NONLINEAR_NV, EGL_DONT_CARE };
		for (unsigned int depthFormat = 0; depthFormat < (sizeof(depthSizes) / sizeof(depthSizes[0])); ++depthFormat) {
			if (!m_EGLExt_DepthNonlinear && depthEncodings[depthFormat] != EGL_DONE_CARE) {
				continue;
			}
			int depthSize = depthSizes[depthFormat], depthEncoding = depthEncodings[depthFormat];
			for (stencilSize = (info.m_bUseStencil ? 8 : 0); stencilSize >= 0; stencilSize -= 8) {
				if (!info.m_bWaitForVSync) {
					config = RequestEGLConfig(depthSize, depthEncoding, stencilSize, 0);
					if (config != nullptr) { break; }
				}
				config = RequestEGLConfig(depthSize, depthEncoding, stencilSize, EGL_DONT_CARE);
				if (config != nullptr) { break; }
			}
			if (config != nullptr) { break; }
		}
		if (config == nullptr) {
			Warning("EGL ERROR: Failed to choose a EGL config.\n");
			ShutdownGLES();
			return UPDATE_GLES_CONTEXT_FAILED;
		}
		configID = 0;
		eglGetConfigAttrib(m_EGLDisplay, config, EGL_CONFIG_ID, &configID);
	} else {
		config = m_EGLConfig;
		configID = m_EGLConfigID;
	}

	m_EGLConfigRequest_UseStencil = info.m_bUseStencil;
	m_EGLConfigRequest_WaitForVSync = info.m_bWaitForVSync;

	if (m_EGLContext != EGL_NO_CONTEXT && m_EGLConfigID != 0 && m_EGLConfigID == configID) {
		return UPDATE_GLES_CONTEXT_KEPT;
	}

	ShutdownGLES();

	if (!eglGetConfigAttrib(m_EGLDisplay, config, EGL_NATIVE_VISUAL_ID, &m_EGLNativeVisualID)) {
		Warning("EGL ERROR: Failed to get the native visual ID of the config.\n");
		return UPDATE_GLES_CONTEXT_FAILED;
	}

	m_EGLConfig = config;
	m_EGLConfigID = configID;

	m_EGLConfigStencilSize = stencilSize; // In case eglGetConfigAttrib fails.
	m_EGLConfigSamples = samples;
	eglGetConfigAttrib(m_EGLDisplay, config, EGL_STENCIL_SIZE, &m_EGLConfigStencilSize);
	eglGetConfigAttrib(m_EGLDisplay, config, EGL_SAMPLES, &m_EGLConfigSamples);

	const int pbufferAttribs[] = { EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE };
	m_EGLPbuffer = eglCreatePbufferSurface(m_EGLDisplay, config, pbufferAttribs);
	if (m_EGLPbuffer == EGL_NO_SURFACE) {
		Warning("EGL ERROR: Failed to create a 1x1 pbuffer surface.\n");
		ShutdownGLES();
		return UPDATE_GLES_CONTEXT_FAILED;
	}

	const int contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	m_EGLContext = eglCreateContext(m_EGLDisplay, config, EGL_NO_CONTEXT, contextAttribs);
	if (m_EGLContext == EGL_NO_CONTEXT) {
		Warning("EGL ERROR: Failed to create an OpenGL ES context.\n");
		ShutdownGLES();
		return UPDATE_GLES_CONTEXT_FAILED;
	}

	return UPDATE_GLES_CONTEXT_CREATED;
}

bool CShaderDeviceMgrEGL::UpdateGLESSurface(void *hWnd, int width, int height) {
	if (m_EGLContext == EGL_NO_CONTEXT) {
		AssertMsg(0, "OpenGL ES 2.0 context needs to be created before updating EGL surface.");
		return false;
	}

	// Force because it may be the first call.
	eglMakeCurrent(m_EGLDisplay, m_EGLPbuffer, m_EGLPbuffer, m_EGLContext);
	m_EGLContextActive = true;

	if (m_EGLSurface != EGL_NO_SURFACE) {
		eglDestroySurface(m_EGLDisplay, m_EGLSurface);
		m_EGLSurface = EGL_NO_SURFACE;
	}
	m_EGLSurfaceWindow = (EGLNativeWindowType) nullptr;

	if (hWnd == nullptr) {
		return true;
	}

	EGLNativeWindowType nativeWindow = (EGLNativeWindowType) hWnd;
#ifdef __ANDROID__
	ANativeWindow_setBuffersGeometry(nativeWindow, width, height, m_EGLConfigNativeVisualID);
#endif
	m_EGLSurface = eglCreateWindowSurface(m_EGLDisplay, m_EGLConfig, nativeWindow, nullptr);
	if (m_EGLSurface == EGL_NO_SURFACE) {
		Warning("EGL ERROR: Failed to create a window surface.\n");
		return false;
	}
	m_EGLSurfaceWindow = nativeWindow;
	eglMakeCurrent(m_EGLDisplay, m_EGLSurface, m_EGLSurface, m_EGLContext);
	m_EGLContextActive = true;
	if (!m_EGLConfigRequest_WaitForVSync) {
		eglSwapInterval(m_EGLDisplay, 0);
	}
	return true;
}

bool CShaderDeviceMgrEGL::ResizeCurrentGLESSurface(int width, int height) {
	if (m_EGLSurfaceWindow == (EGLNativeWindowType) nullptr) {
		return true;
	}
	return UpdateGLESSurface(reinterpet_cast<void *>(m_EGLSurfaceWindow), width, height);
}

bool CShaderDeviceMgrEGL::IsGLESContextActive() const {
	return m_EGLContextActive;
}

int CShaderDeviceMgrEGL::StencilBufferBits() const {
	return m_EGLContextStencilSize;
}

bool CShaderDeviceMgrEGL::IsAAEnabled() const {
	return m_EGLContextSamples > 1;
}

void CShaderDeviceMgrEGL::Present() {
	if (m_EGLContextActive && m_EGLSurface != EGL_NO_SURFACE) {
		eglSwapBuffers(m_EGLDisplay, m_EGLSurface);
	}
}

void CShaderDeviceMgrEGL::ShutdownGLES() {
	if (m_EGLContext != EGL_NO_CONTEXT) {
		g_pShaderAPI->OnGLESContextShutdown();
		m_EGLContextActive = false;
		eglMakeCurrent(m_EGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		eglDestroyContext(m_EGLDisplay, m_EGLContext);
		m_EGLContext = EGL_NO_CONTEXT;
	}
	if (m_EGLSurface != EGL_NO_SURFACE) {
		eglDestroySurface(m_EGLDisplay, m_EGLSurface);
		m_EGLSurface = EGL_NO_SURFACE;
	}
	if (m_EGLPbuffer != EGL_NO_SURFACE) {
		eglDestroySurface(m_EGLDisplay, m_EGLPbuffer);
		m_EGLPbuffer = EGL_NO_SURFACE;
	}
	m_EGLConfig = nullptr;
	m_EGLConfigID = 0;
}
