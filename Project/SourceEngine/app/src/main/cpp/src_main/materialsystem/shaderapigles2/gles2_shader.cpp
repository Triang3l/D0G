// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#include "gles2_api.h"
#include "tier0/dbg.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// Unused methods.

IShaderBuffer *CShaderAPIGLES2::CompileShader(const char *pProgram, size_t nBufLen, const char *pShaderVersion) {
	AssertMsg(0, "Can't compile Direct3D shaders with an OpenGL ES ShaderAPI.");
	return NULL;
}

VertexShaderHandle_t CShaderAPIGLES2::CreateVertexShader(IShaderBuffer *pShaderBuffer) {
	AssertMsg(0, "Shaders must be created as programs.");
	return VERTEX_SHADER_HANDLE_INVALID;
}

void CShaderAPIGLES2::DestroyVertexShader(VertexShaderHandle_t hShader) {
	AssertMsg(0, "Shaders must be destroyed as programs.");
}

GeometryShaderHandle_t CShaderAPIGLES2::CreateGeometryShader(IShaderBuffer *pShaderBuffer) {
	AssertMsg(0, "Shaders must be created as programs.");
	return GEOMETRY_SHADER_HANDLE_INVALID;
}

void CShaderAPIGLES2::DestroyGeometryShader(GeometryShaderHandle_t hShader) {
	AssertMsg(0, "Shaders must be destroyed as programs.");
}

PixelShaderHandle_t CShaderAPIGLES2::CreatePixelShader(IShaderBuffer *pShaderBuffer) {
	AssertMsg(0, "Shaders must be created as programs.");
	return PIXEL_SHADER_HANDLE_INVALID;
}

void CShaderAPIGLES2::DestroyPixelShader(PixelShaderHandle_t hShader) {
	AssertMsg(0, "Shaders must be destroyed as programs.");
}

void CShaderAPIGLES2::BindVertexShader(VertexShaderHandle_t hVertexShader) {
	AssertMsg(0, "Shaders must be bound using the transition table as programs.");
}

void CShaderAPIGLES2::BindGeometryShader(GeometryShaderHandle_t hGeometryShader) {
	AssertMsg(0, "Shaders must be bound using the transition table as programs.");
}

void CShaderAPIGLES2::BindPixelShader(PixelShaderHandle_t hPixelShader) {
	AssertMsg(0, "Shaders must be bound using the transition table as programs.");
}
