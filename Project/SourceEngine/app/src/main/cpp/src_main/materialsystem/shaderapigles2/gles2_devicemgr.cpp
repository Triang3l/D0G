// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#include "gles2_devicemgr.h"
#include "gles2_glfuncs.h"
#include "gles2_hardwareconfig.h"
#include "materialsystem/imaterialsystem.h"
#include "shaderapi/ishaderapi.h"
#include "shaderapi/ishadershadow.h"
#include "tier0/dbg.h"
#include "tier1/strtools.h"
#include <string.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CShaderDeviceMgrBase *g_pShaderDeviceMgr;

static GLES2Funcs_t s_GL;
GLES2Funcs_t *g_pGL = &s_GL;

CShaderDeviceMgrBase() {
	m_GLESVendorAndRenderer[0] = '\0';
}

static CreateInterfaceFn s_TempFactory;

void *ShaderDeviceFactory(const char *name, int *returnCode) {
	if (returnCode != NULL) {
		*returnCode = IFACE_OK;
	}

	void *interface = s_TempFactory(name, returnCode);
	if (interface != NULL) {
		return interface;
	}

	interface = Sys_GetFactoryThis()(name, returnCode);
	if (interface != NULL) {
		return interface;
	}

	if (returnCode != NULL) {
		*returnCode = IFACE_FAILED;
	}
	return NULL;
}

bool CShaderDeviceMgrBase::Connect(CreateInterfaceFn factory) {
	Assert(!g_pShaderDeviceMgr);

	s_TempFactory = factory;
	CreateInterfaceFn actualFactory = ShaderDeviceFactory;
	ConnectTier1Libraries(&actualFactory, 1);
	InitShaderAPICVars();
	ConnectTier3Libraries(&actualFactory, 1);
	g_pShaderDeviceMgr = this;
	s_TempFactory = NULL;

	MathLib_Init(2.2f, 2.2f, 0.0f, 2.0f);
	return true;
}

void CShaderDeviceMgrBase::Disconnect() {
	g_pShaderDeviceMgr = NULL;
	DisconnectTier2Libraries();
	ConVar_Unregister();
	DisconnectTier1Libraries();
}

InitReturnVal_t CShaderDeviceMgrBase::Init() {
	// Window system interface.
	if (!InitWindowSystemInterface()) {
		return INIT_FAILED;
	}

	// OpenGL ES 2.0 core.
	if (!InitGLES2Library()) {
		Warning("ERROR: Failed to open libGLESv2.\n");
		ShutdownWindowSystemInterface();
		return INIT_FAILED;
	}
	if (!InitGLES2CoreFunctions()) {
		ShutdownGLES2Library();
		ShutdownWindowSystemInterface();
		return INIT_FAILED;
	}

	// OpenGL ES 2.0/3.0 capabilities and extensions.
	if (!CreateInitGLESContext()) {
		ShutdownGLES2Library();
		ShutdownWindowSystemInterface();
		return INIT_FAILED;
	}
	g_pHardwareConfig->SetupHardwareCaps();
	InitGLES2ExtensionsAndGLES3Core();
	DestroyInitGLESContext();

	return INIT_OK;
}

bool CShaderDeviceMgrBase::InitGLES2CoreFunctions() {
#define LOAD_GLES2_CORE(function) \
	*((void **) (&(g_pGL->##function))) = GetGLES2CoreFunction("gl"#function); \
	if (g_pGL->##function == NULL) { \
		Warning("ERROR: %s not found in libGLESv2.\n", "gl"#function); \
		return false; \
	}
	LOAD_GLES2_CORE(ActiveTexture)
	LOAD_GLES2_CORE(AttachShader)
	LOAD_GLES2_CORE(BindAttribLocation)
	LOAD_GLES2_CORE(BindBuffer)
	LOAD_GLES2_CORE(BindFramebuffer)
	LOAD_GLES2_CORE(BindRenderbuffer)
	LOAD_GLES2_CORE(BindTexture)
	LOAD_GLES2_CORE(BlendColor)
	LOAD_GLES2_CORE(BlendEquation)
	LOAD_GLES2_CORE(BlendEquationSeparate)
	LOAD_GLES2_CORE(BlendFunc)
	LOAD_GLES2_CORE(BlendFuncSeparate)
	LOAD_GLES2_CORE(BufferData)
	LOAD_GLES2_CORE(BufferSubData)
	LOAD_GLES2_CORE(CheckFramebufferStatus)
	LOAD_GLES2_CORE(Clear)
	LOAD_GLES2_CORE(ClearColor)
	LOAD_GLES2_CORE(ClearDepthf)
	LOAD_GLES2_CORE(ClearStencil)
	LOAD_GLES2_CORE(ColorMask)
	LOAD_GLES2_CORE(CompileShader)
	LOAD_GLES2_CORE(CompressedTexImage2D)
	LOAD_GLES2_CORE(CompressedTexSubImage2D)
	LOAD_GLES2_CORE(CopyTexImage2D)
	LOAD_GLES2_CORE(CopyTexSubImage2D)
	LOAD_GLES2_CORE(CreateProgram)
	LOAD_GLES2_CORE(CreateShader)
	LOAD_GLES2_CORE(CullFace)
	LOAD_GLES2_CORE(DeleteBuffers)
	LOAD_GLES2_CORE(DeleteFramebuffers)
	LOAD_GLES2_CORE(DeleteProgram)
	LOAD_GLES2_CORE(DeleteRenderbuffers)
	LOAD_GLES2_CORE(DeleteShader)
	LOAD_GLES2_CORE(DeleteTextures)
	LOAD_GLES2_CORE(DepthFunc)
	LOAD_GLES2_CORE(DepthMask)
	LOAD_GLES2_CORE(DepthRangef)
	LOAD_GLES2_CORE(DetachShader)
	LOAD_GLES2_CORE(Disable)
	LOAD_GLES2_CORE(DisableVertexAttribArray)
	LOAD_GLES2_CORE(DrawArrays)
	LOAD_GLES2_CORE(DrawElements)
	LOAD_GLES2_CORE(Enable)
	LOAD_GLES2_CORE(EnableVertexAttribArray)
	LOAD_GLES2_CORE(Finish)
	LOAD_GLES2_CORE(Flush)
	LOAD_GLES2_CORE(FramebufferRenderbuffer)
	LOAD_GLES2_CORE(FramebufferTexture2D)
	LOAD_GLES2_CORE(FrontFace)
	LOAD_GLES2_CORE(GenBuffers)
	LOAD_GLES2_CORE(GenerateMipmap)
	LOAD_GLES2_CORE(GenFramebuffers)
	LOAD_GLES2_CORE(GenRenderbuffers)
	LOAD_GLES2_CORE(GenTextures)
	LOAD_GLES2_CORE(GetActiveAttrib)
	LOAD_GLES2_CORE(GetActiveUniform)
	LOAD_GLES2_CORE(GetAttachedShaders)
	LOAD_GLES2_CORE(GetAttribLocation)
	LOAD_GLES2_CORE(GetBooleanv)
	LOAD_GLES2_CORE(GetBufferParameteriv)
	LOAD_GLES2_CORE(GetError)
	LOAD_GLES2_CORE(GetFloatv)
	LOAD_GLES2_CORE(GetFramebufferAttachmentParameteriv)
	LOAD_GLES2_CORE(GetIntegerv)
	LOAD_GLES2_CORE(GetProgramiv)
	LOAD_GLES2_CORE(GetProgramInfoLog)
	LOAD_GLES2_CORE(GetRenderbufferParameteriv)
	LOAD_GLES2_CORE(GetShaderiv)
	LOAD_GLES2_CORE(GetShaderInfoLog)
	LOAD_GLES2_CORE(GetShaderPrecisionFormat)
	LOAD_GLES2_CORE(GetShaderSource)
	LOAD_GLES2_CORE(GetString)
	LOAD_GLES2_CORE(GetTexParameterfv)
	LOAD_GLES2_CORE(GetTexParameteriv)
	LOAD_GLES2_CORE(GetUniformfv)
	LOAD_GLES2_CORE(GetUniformiv)
	LOAD_GLES2_CORE(GetUniformLocation)
	LOAD_GLES2_CORE(GetVertexAttribfv)
	LOAD_GLES2_CORE(GetVertexAttribiv)
	LOAD_GLES2_CORE(GetVertexAttribPointerv)
	LOAD_GLES2_CORE(Hint)
	LOAD_GLES2_CORE(IsBuffer)
	LOAD_GLES2_CORE(IsEnabled)
	LOAD_GLES2_CORE(IsFramebuffer)
	LOAD_GLES2_CORE(IsProgram)
	LOAD_GLES2_CORE(IsRenderbuffer)
	LOAD_GLES2_CORE(IsShader)
	LOAD_GLES2_CORE(IsTexture)
	LOAD_GLES2_CORE(LineWidth)
	LOAD_GLES2_CORE(LinkProgram)
	LOAD_GLES2_CORE(PixelStorei)
	LOAD_GLES2_CORE(PolygonOffset)
	LOAD_GLES2_CORE(ReadPixels)
	LOAD_GLES2_CORE(ReleaseShaderCompiler)
	LOAD_GLES2_CORE(RenderbufferStorage)
	LOAD_GLES2_CORE(SampleCoverage)
	LOAD_GLES2_CORE(Scissor)
	LOAD_GLES2_CORE(ShaderBinary)
	LOAD_GLES2_CORE(ShaderSource)
	LOAD_GLES2_CORE(StencilFunc)
	LOAD_GLES2_CORE(StencilFuncSeparate)
	LOAD_GLES2_CORE(StencilMask)
	LOAD_GLES2_CORE(StencilMaskSeparate)
	LOAD_GLES2_CORE(StencilOp)
	LOAD_GLES2_CORE(StencilOpSeparate)
	LOAD_GLES2_CORE(TexImage2D)
	LOAD_GLES2_CORE(TexParameterf)
	LOAD_GLES2_CORE(TexParameterfv)
	LOAD_GLES2_CORE(TexParameteri)
	LOAD_GLES2_CORE(TexParameteriv)
	LOAD_GLES2_CORE(TexSubImage2D)
	LOAD_GLES2_CORE(Uniform1f)
	LOAD_GLES2_CORE(Uniform1fv)
	LOAD_GLES2_CORE(Uniform1i)
	LOAD_GLES2_CORE(Uniform1iv)
	LOAD_GLES2_CORE(Uniform2f)
	LOAD_GLES2_CORE(Uniform2fv)
	LOAD_GLES2_CORE(Uniform2i)
	LOAD_GLES2_CORE(Uniform2iv)
	LOAD_GLES2_CORE(Uniform3f)
	LOAD_GLES2_CORE(Uniform3fv)
	LOAD_GLES2_CORE(Uniform3i)
	LOAD_GLES2_CORE(Uniform3iv)
	LOAD_GLES2_CORE(Uniform4f)
	LOAD_GLES2_CORE(Uniform4fv)
	LOAD_GLES2_CORE(Uniform4i)
	LOAD_GLES2_CORE(Uniform4iv)
	LOAD_GLES2_CORE(UniformMatrix2fv)
	LOAD_GLES2_CORE(UniformMatrix3fv)
	LOAD_GLES2_CORE(UniformMatrix4fv)
	LOAD_GLES2_CORE(UseProgram)
	LOAD_GLES2_CORE(ValidateProgram)
	LOAD_GLES2_CORE(VertexAttrib1f)
	LOAD_GLES2_CORE(VertexAttrib1fv)
	LOAD_GLES2_CORE(VertexAttrib2f)
	LOAD_GLES2_CORE(VertexAttrib2fv)
	LOAD_GLES2_CORE(VertexAttrib3f)
	LOAD_GLES2_CORE(VertexAttrib3fv)
	LOAD_GLES2_CORE(VertexAttrib4f)
	LOAD_GLES2_CORE(VertexAttrib4fv)
	LOAD_GLES2_CORE(VertexAttribPointer)
	LOAD_GLES2_CORE(Viewport)
	return true;
}

void CShaderDeviceMgrBase::InitGLES2ExtensionsAndGLES3Core() {
}

int CShaderDeviceMgrBase::GetAdapterCount() const {
	return 1;
}

void CShaderDeviceMgrBase::GetAdapterInfo(int nAdapter, MaterialAdapterInfo_t &info) const {
	memcpy(&info, static_cast<const MaterialAdapterInfo_t *>(&g_pHardwareConfig->Caps()),
			sizeof(MaterialAdapterInfo_t));
}

bool CShaderDeviceMgrBase::GetRecommendedConfigurationInfo(int nAdapter, int nDXLevel, KeyValues *pConfiguration) {
	return true;
}

CreateInterfaceFn CShaderDeviceMgrBase::SetMode(void *hWnd, int nAdapter, const ShaderDeviceInfo_t &mode) {
	return (ShaderAPI()->SetMode(hWnd, nAdapter, mode) ? ShaderInterfaceFactory : NULL);
}

// EGL doesn't provide info about modes, and on Android, the size is obtained from a window.

int CShaderDeviceMgrBase::GetModeCount(int nAdapter) const {
	return 1;
}

void CShaderDeviceMgrBase::GetModeInfo(ShaderDisplayMode_t *pInfo, int nAdapter, int nMode) const {
	Assert(pInfo->m_nVersion == SHADER_DISPLAY_MODE_VERSION);
	pInfo->m_nWidth = 640;
	pInfo->m_nHeight = 480;
	pInfo->m_Format = IMAGE_FORMAT_RGBA8888;
	pInfo->m_nRefreshRateNumerator = 60;
	pInfo->m_nRefreshRateDenominator = 1;
}

void CShaderDeviceMgrBase::GetCurrentModeInfo(ShaderDisplayMode_t *pInfo, int nAdapter) const {
	GetModeInfo(pInfo, nAdapter, 0);
}

void *CShaderDeviceMgrBase::ShaderInterfaceFactory(const char *pInterfaceName, int *pReturnCode) {
	if (pReturnCode != NULL) {
		*pReturnCode = IFACE_OK;
	}
	if (Q_stricmp(pInterfaceName, SHADER_DEVICE_INTERFACE_VERSION) == 0) {
		return static_cast<IShaderDevice *>(g_pShaderDevice);
	}
	if (Q_stricmp(pInterfaceName, SHADERAPI_INTERFACE_VERSION) == 0) {
		return static_cast<IShaderAPI *>(g_pShaderAPI);
	}
	if (Q_stricmp(pInterfaceName, SHADERSHADOW_INTERFACE_VERSION) == 0) {
		return static_cast<IShaderShadow *>(g_pShaderShadow);
	}

	if (pReturnCode != NULL) {
		*pReturnCode = IFACE_FAILED;
	}
	return NULL;
}
