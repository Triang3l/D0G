// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#include "gles2_shader.h"
#include "gles2_api.h"
#include "gles2_hardwareconfig.h"
#include "tier0/dbg.h"
#include "tier0/platform.h"
#include <stdlib.h>
#include <string.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static CShaderManager s_ShaderManager;
CShaderManager *g_pShaderManager = &s_ShaderManager;

CShaderManager::CShaderManager() :
		m_ShaderPrograms(256),
		m_ShaderProgramStaticCombos(256),
		m_ShaderProgramStaticComboDict(256),
		m_NewProgramVertexDynamicIndices(0, 16),
		m_NewProgramPixelDynamicIndices(0, 16),
		m_NewProgramVertexShaderSource(0, 32),
		m_NewProgramPixelShaderSource(0, 32) {}

bool CShaderManager::ShaderProgramStaticComboLessFunc::Less(const ShaderProgramStaticComboDictEntry_t &entry1,
		const ShaderProgramStaticComboDictEntry_t &entry2, void *context) {
	return entry1.m_Checksum < entry2.m_Checksum;
}

void CShaderManager::DynamicIndexComparison(const int *combo1, const int *combo2) {
	return *combo1 - *combo2;
}

ShaderProgramStaticComboHandle_t CShaderManager::GetVertexPixelProgramStaticCombo(
			const char *vertexProviderName, int vertexStaticIndex,
			const char *pixelProviderName, int pixelStaticIndex) {
	ShaderProgramStaticComboDictEntry_t comboEntry;
	CRC32_Init(&comboEntry.m_Checksum);
	CRC32_ProcessBuffer(&comboEntry.m_Checksum, vertexProviderName, strlen(vertexProviderName) + 1);
	CRC32_ProcessBuffer(&comboEntry.m_Checksum, &vertexStaticIndex, sizeof(vertexStaticIndex));
	CRC32_ProcessBuffer(&comboEntry.m_Checksum, pixelProviderName, strlen(pixelProviderName) + 1);
	CRC32_ProcessBuffer(&comboEntry.m_Checksum, &pixelStaticIndex, sizeof(pixelStaticIndex));
	CRC32_Final(&comboEntry.m_Checksum);

	// Searching for this static combo that was created previously.
	ShaderProgramStaticComboHandle_t comboHandle = SHADER_PROGRAM_STATIC_COMBO_HANDLE_INVALID;
	int dictIndex = m_ShaderProgramStaticComboDict.FindLessOrEqual(comboEntry);
	if (dictIndex >= 0) {
		int dictCount = m_ShaderProgramStaticComboDict.Count();
		for (; dictIndex < dictCount; ++i) {
			const ShaderProgramStaticComboDictEntry_t &foundEntry = m_ShaderProgramStaticComboDict[dictIndex];
			if (foundEntry.m_Checksum > comboEntry.m_Checksum) {
				break;
			}
			if (foundEntry.m_Checksum != comboEntry.m_Checksum) {
				continue;
			}
			const ShaderProgramStaticCombo_t &foundCombo = m_ShaderProgramStaticCombos[foundEntry.m_Combo];
			if (foundCombo.m_VertexStaticIndex == vertexStaticIndex && foundCombo.m_PixelStaticIndex == pixelStaticIndex &&
					strcmp(foundCombo.m_VertexProvider->GetName(), vertexProviderName) == 0 &&
					strcmp(foundCombo.m_PixelProvider->GetName(), pixelProviderName) == 0) {
				comboHandle = foundEntry.m_Combo;
				break;
			}
		}
	}
	if (comboHandle != SHADER_PROGRAM_STATIC_COMBO_HANDLE_INVALID) {
		return comboHandle;
	}

	// Combo not found - need to create a new one. Get the providers.
	const IShaderSourceProvider *vertexProvider = g_pShaderUtil->FindShaderSourceProvider(
			IShaderSourceProvider::SHADER_STAGE_VERTEX, vertexProviderName);
	const IShaderSourceProvider *pixelProvider = g_pShaderUtil->FindShaderSourceProvider(
			IShaderSourceProvider::SHADER_STAGE_PIXEL, pixelProviderName);
	if (vertexProvider == NULL || pixelProvider == NULL) {
		if (vertexProvider == NULL) {
			Warning("Vertex shader provider %s not found!\n", vertexProviderName);
		}
		if (pixelProvider == NULL) {
			Warning("Pixel shader provider %s not found!\n", pixelProviderName);
		}
		return SHADER_PROGRAM_STATIC_COMBO_HANDLE_INVALID;
	}

	// Getting the dynamic combos to compile.
	m_NewProgramVertexDynamicIndices.RemoveAll();
	vertexProvider->GetDynamicCombos(vertexStaticIndex, m_NewProgramVertexDynamicIndices);
	m_NewProgramPixelDynamicIndices.RemoveAll();
	pixelProvider->GetDynamicCombos(pixelStaticIndex, m_NewProgramPixelDynamicIndices);
	int vertexDynamicComboCount = m_NewProgramVertexDynamicIndices.Count();
	int pixelDynamicComboCount = m_NewProgramPixelDynamicIndices.Count();
	if (vertexDynamicComboCount == 0 || pixelDynamicComboCount == 0) {
		if (vertexDynamicComboCount == 0) {
			Warning("Vertex shader %s:%x has no dynamic combos specified by the provider!\n",
					vertexProviderName, vertexStaticIndex);
		}
		if (pixelDynamicComboCount == 0) {
			Warning("Pixel shader %s:%x has no dynamic combos specified by the provider!\n",
					pixelProviderName, pixelStaticIndex);
		}
		return SHADER_PROGRAM_STATIC_COMBO_HANDLE_INVALID;
	}

	// Binary search is used when binding dynamic combos, so they need to be sorted.
	m_NewProgramVertexDynamicIndices.Sort(DynamicIndexComparison);
	m_NewProgramPixelDynamicIndices.Sort(DynamicIndexComparison);

	// Compiling the programs.
	ShaderProgramHandle_t firstProgramHandle = m_ShaderPrograms.AddMultipleToTail(vertexDynamicComboCount * pixelDynamicComboCount);
	ShaderProgramHandle_t programHandle = firstProgramHandle;
	for (int vertexDynamicComboIndex = 0; vertexDynamicComboIndex < vertexDynamicComboCount; ++vertexDynamicComboIndex) {
		for (int pixelDynamicComboIndex = 0; pixelDynamicComboIndex < pixelDynamicComboCount; ++pixelDynamicComboIndex) {
			CompileVertexPixelProgram(programHandle++,
					vertexProvider, vertexStaticIndex, m_NewProgramVertexDynamicIndices[vertexDynamicComboIndex],
					pixelProvider, pixelStaticIndex, m_NewProgramVertexDynamicIndices[pixelDynamicComboIndex]);
		}
	}

	// Adding the static combo to the dictionary.
	comboHandle = m_ShaderProgramStaticCombos.AddToTail();
	ShaderProgramStaticCombo_t &combo = m_ShaderProgramStaticCombos[comboHandle];
	combo.m_VertexProvider = vertexProvider;
	combo.m_PixelProvider = pixelProvider;
	combo.m_VertexStaticIndex = vertexStaticIndex;
	combo.m_PixelStaticIndex = pixelStaticIndex;
	combo.m_FirstDynamicCombo = firstProgramHandle;
	combo.m_VertexDynamicComboCount = vertexDynamicComboCount;
	combo.m_PixelDynamicComboCount = pixelDynamicComboCount;
	comboEntry.m_Combo = comboHandle;
	m_ShaderProgramStaticComboDict.Insert(comboEntry);

	return comboHandle;
}

GLuint CShaderManager::CompileShader(const IShaderSourceProvider *provider, int staticIndex, int dynamicIndex) {
	IShaderSourceProvider::ShaderStage_t stage = provider->GetStage();
	GLenum creationStage;
	const char *errorStage;
	CUtlVector<const char *> *source;
	if (stage == IShaderSourceProvider::SHADER_STAGE_VERTEX) {
		creationStage = GL_VERTEX_SHADER;
		errorStage = "vertex";
		source = &m_NewProgramVertexShaderSource;
	} else if (stage == IShaderSourceProvider::SHADER_STAGE_PIXEL) {
		creationStage = GL_FRAGMENT_SHADER;
		errorStage = "pixel";
		source = &m_NewProgramPixelShaderSource;
	} else {
		Warning("Attempted to create a shader %s:%x:%x for an unknown pipeline stage (%d)!\n",
				provider->GetName(), staticIndex, dynamicIndex, stage);
		Assert(0);
		return 0;
	}

	GLuint shader = g_pGL->CreateShader(creationStage);
	if (shader == 0) {
		Warning("Failed to create a GL %s shader object for %s:%x:%x.\n", errorStage,
				provider->GetName(), staticIndex, dynamicIndex);
		return 0;
	}

	source->RemoveAll();

	const HardwareCaps_t &caps = g_pHardwareConfig->Caps();
	int glesVersion = caps.m_GLESVersion;
	const char *versionString, *textureDefinitions;
	if (glesVersion >= 300) {
		if (glesVersion >= 310) {
			source->AddToTail("#version 310 es\n");
		} else {
			source->AddToTail("#version 300 es\n");
		}
		source->AddToTail("#define v_texture2D texture\n"
				"#define v_textureCube texture\n"
				"#define v_texture3D texture\n"
				"#define v_shadow texture\n");
		if (stage == IShaderSourceProvider::SHADER_STAGE_VERTEX) {
			source->AddToTail("#define v_attribute in\n"
					"#define v_varying out\n");
		} else if (stage == IShaderSourceProvider::SHADER_STAGE_PIXEL) {
			source->AddToTail("#define v_highp highp\n"
					"precision highp float;\n"
					"#define v_varying in\n"
					"layout(location = 0) out vec4 v_FragColor;\n");
		}
	} else {
		source->AddToTail("#version 100\n");
		if (caps.m_Ext_ShadowSamplers) {
			source->AddToTail("#extension GL_EXT_shadow_samplers : enable\n");
		}
		if (caps.m_Ext_Texture3D) {
			source->AddToTail("#extension GL_OES_texture_3D : enable\n");
		}
		source->AddToTail("#define v_texture2D texture2D\n"
				"#define v_textureCube textureCube\n"
				"#define v_texture3D texture3D\n"
				"#define v_shadow shadow2DEXT\n");
		if (stage == IShaderSourceProvider::SHADER_STAGE_VERTEX) {
			source->AddToTail("#define v_attribute attribute\n"
					"#define v_varying varying\n");
		} else if (stage == IShaderSourceProvider::SHADER_STAGE_PIXEL) {
			if (caps.m_Ext_FragmentPrecisionHigh) {
				source->AddToTail("#define v_highp highp\n");
			} else {
				source->AddToTail("#define v_highp mediump\n");
			}
			source->AddToTail("precision v_highp float;\n"
					"#define v_varying varying\n"
					"#define v_FragColor gl_FragColor\n");
		}
	}

	provider->GetSource(staticIndex, dynamicIndex, *source);

	int partCount = source->Count();
	g_pGL->ShaderSource(shader, partCount, source->Base(), NULL);
	g_pGL->CompileShader(shader);
	int compileStatus = GL_FALSE;
	g_pGL->GetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (!compileStatus) {
		Warning("Failed to compile %s shader %s:%x:%x!\n", errorStage,
				provider->GetName(), staticIndex, dynamicIndex);
		int infoLogLength = 0;
		g_pGL->GetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 1) {
			char infoLog[1024];
			if (infoLogLength > sizeof(infoLog)) {
				infoLogLength = sizeof(infoLog);
			}
			infoLog[0] = '\0';
			g_pGL->GetShaderInfoLog(shader, infoLogLength, NULL, infoLog);
			infoLog[infoLogLength - 1] = '\0';
			Warning("%s\n", infoLog);
		}
		Warning("Source code:\n");
		for (int partIndex = 0; partIndex < partCount; ++partIndex) {
			Warning("// Part %d\n%s", partIndex, source->Element(partIndex));
		}
		g_pGL->DeleteShader(shader);
		return 0;
	}

	return shader;
}

bool CShaderManager::CompileVertexPixelProgram(ShaderProgramHandle_t handle,
		const IShaderSourceProvider *vertexProvider, int vertexStaticIndex, int vertexDynamicIndex,
		const IShaderSourceProvider *pixelProvider, int pixelStaticIndex, int pixelDynamicIndex) {
	ShaderProgram_t &program = m_ShaderPrograms[handle];

	// Initializing variables to reduce the possibility of issues if the program fails to compile.
	// VertexDynamicIndex and PixelDynamicIndex are required for searching.
	program.m_VertexDynamicIndex = vertexDynamicIndex;
	program.m_PixelDynamicIndex = pixelDynamicIndex;
	program.m_GLProgram = 0;
	program.m_GLVertexShader = 0;
	program.m_GLPixelShader = 0;
	memset(program.m_VertexAttributes, 0xff, sizeof(program.m_ShaderSpecificVertexConstants));
	for (int standardConstantIndex = 0; standardConstantIndex < STANDARD_CONSTANT_COUNT; ++standardConstantIndex) {
		ShaderProgram_t::StandardConstant_t &standardConstant = program.m_StandardConstants[standardConstantIndex];
		standardConstant.m_Location = -1;
		standardConstant.m_LastUpdate = 0;
	}
	memset(program.m_ShaderSpecificVertexConstants, 0xff, sizeof(program.m_ShaderSpecificVertexConstants));
	memset(program.m_ShaderSpecificPixelConstants, 0xff, sizeof(program.m_ShaderSpecificPixelConstants));

	// Compiling the shaders.
	GLuint vertexShaderGLHandle = CompileShader(vertexProvider, vertexStaticIndex, vertexDynamicIndex);
	if (vertexShaderGLHandle == 0) {
		return false;
	}
	GLuint pixelShaderGLHandle = CompileShader(pixelProvider, pixelStaticIndex, pixelDynamicIndex);
	if (pixelShaderGLHandle == 0) {
		g_pGL->DeleteShader(vertexShaderGLHandle);
		return false;
	}

	// Creating and linking the program.
	GLuint programGLHandle = g_pGL->CreateProgram();
	if (programGLHandle == 0) {
		Warning("Failed to create a GL program object for program with vertex shader %s:%x:%x and pixel shader %s:%x:%x.\n",
				vertexProvider->GetName(), vertexStaticIndex, vertexDynamicIndex,
				pixelProvider->GetName(), pixelStaticIndex, pixelDynamicIndex);
		g_pGL->DeleteShader(pixelShaderGLHandle);
		g_pGL->DeleteShader(vertexShaderGLHandle);
		return false;
	}
	g_pGL->AttachShader(programGLHandle, vertexShaderGLHandle);
	g_pGL->AttachShader(programGLHandle, pixelShaderGLHandle);
	g_pGL->LinkProgram(programGLHandle);
	int linkStatus = GL_FALSE;
	g_pGL->GetProgramiv(programGLHandle, GL_LINK_STATUS, &linkStatus);
	if (!linkStatus) {
		Warning("Failed to link program with vertex shader %s:%x:%x and pixel shader %s:%x:%x!\n",
				vertexProvider->GetName(), vertexStaticIndex, vertexDynamicIndex,
				pixelProvider->GetName(), pixelStaticIndex, pixelDynamicIndex);
		int infoLogLength = 0;
		g_pGL->GetProgramiv(programGLHandle, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 1) {
			char infoLog[1024];
			if (infoLogLength > sizeof(infoLog)) {
				infoLogLength = sizeof(infoLog);
			}
			infoLog[0] = '\0';
			g_pGL->GetProgramInfoLog(programGLHandle, infoLogLength, NULL, infoLog);
			infoLog[infoLogLength - 1] = '\0';
			Warning("%s\n", infoLog);
		}
		Warning("Vertex shader source code:\n");
		int partCount = m_NewProgramVertexShaderSource.Count();
		for (int partIndex = 0; partIndex < partCount; ++partIndex) {
			Warning("// Part %d\n%s", partIndex, m_NewProgramVertexShaderSource[partIndex]);
		}
		Warning("Pixel shader source code:\n");
		int partCount = m_NewProgramPixelShaderSource.Count();
		for (int partIndex = 0; partIndex < partCount; ++partIndex) {
			Warning("// Part %d\n%s", partIndex, m_NewProgramPixelShaderSource[partIndex]);
		}
		g_pGL->DeleteProgram(programGLHandle);
		g_pGL->DeleteShader(pixelShaderGLHandle);
		g_pGL->DeleteShader(vertexShaderGLHandle);
		return false;
	}
	program.m_GLProgram = programGLHandle;
	program.m_GLVertexShader = vertexShaderGLHandle;
	program.m_GLPixelShader = pixelShaderGLHandle;

	// Vertex attribute locations.
	program.m_VertexAttributes[VERTEX_GLES2_ATTRIBUTE_POSITION] = g_pGL->GetAttribLocation(programGLHandle, "a_Position");
	program.m_VertexAttributes[VERTEX_GLES2_ATTRIBUTE_NORMAL] = g_pGL->GetAttribLocation(programGLHandle, "a_Normal");
	program.m_VertexAttributes[VERTEX_GLES2_ATTRIBUTE_TANGENT] = g_pGL->GetAttribLocation(programGLHandle, "a_Tangent");
	program.m_VertexAttributes[VERTEX_GLES2_ATTRIBUTE_BONEINDEX] = g_pGL->GetAttribLocation(programGLHandle, "a_BoneIndices");
	program.m_VertexAttributes[VERTEX_GLES2_ATTRIBUTE_BONEWEIGHT] = g_pGL->GetAttribLocation(programGLHandle, "a_BoneWeights");
	program.m_VertexAttributes[VERTEX_GLES2_ATTRIBUTE_COLOR] = g_pGL->GetAttribLocation(programGLHandle, "a_Color");
	program.m_VertexAttributes[VERTEX_GLES2_ATTRIBUTE_SPECULAR] = g_pGL->GetAttribLocation(programGLHandle, "a_Specular");
	program.m_VertexAttributes[VERTEX_GLES2_ATTRIBUTE_TEXCOORD_FIRST] = g_pGL->GetAttribLocation(programGLHandle, "a_TexCoord");
	char texCoordAttribute[32] = "a_TexCoord";
	for (int texCoordIndex = 1; texCoordIndex < VERTEX_MAX_TEXTURE_COORDINATES; ++texCoordIndex) {
		itoa(texCoordIndex, texCoordAttribute + (sizeof("a_TexCoord") - 1), 10);
		program.m_VertexAttributes[VERTEX_GLES2_ATTRIBUTE_TEXCOORD_FIRST + texCoordIndex] =
				g_pGL->GetAttribLocation(programGLHandle, texCoordAttribute);
	}
#ifdef SHADERAPI_GLES2_HARDWARE_MORPH
	program.m_VertexAttributes[VERTEX_GLES2_ATTRIBUTE_FLEXPOSITION] = g_pGL->GetAttribLocation(programGLHandle, "a_FlexPosition");
	program.m_VertexAttributes[VERTEX_GLES2_ATTRIBUTE_FLEXNORMAL] = g_pGL->GetAttribLocation(programGLHandle, "a_FlexNormal");
#endif
	// Using a 32-bit mask of enabled vertex attributes. Extremely unlikely that a shader will have over
	// 32 attributes and they will get location 32 and above, but still.
	for (int attributeIndex = 0; attributeIndex < VERTEX_GLES2_ATTRIBUTE_COUNT; ++attributeIndex) {
		if (program.m_VertexAttributes[attributeIndex] >= 32) {
			program.m_VertexAttributes[attributeIndex] = -1;
		}
	}

	// Standard constant locations.
	program.m_StandardConstants[STANDARD_CONSTANT_MODELVIEWPROJ] = g_pGL->GetUniformLocation(programGLHandle, "g_ModelViewProj");
	program.m_StandardConstants[STANDARD_CONSTANT_VIEWPROJ] = g_pGL->GetUniformLocation(programGLHandle, "g_ViewProj");
	program.m_StandardConstants[STANDARD_CONSTANT_VIEWMODEL] = g_pGL->GetUniformLocation(programGLHandle, "g_ViewModel");

	// Shader-specific constant locations.
	const char *shaderSpecificConstants[IShaderSourceProvider::MAX_SHADER_STAGE_SPECIFIC_CONSTANTS];
	memset(shaderSpecificConstants, 0, sizeof(shaderSpecificConstants));
	vertexProvider->GetConstants(vertexStaticIndex, vertexDynamicIndex, shaderSpecificConstants);
	for (int constantIndex = 0; constantIndex < IShaderSourceProvider::MAX_SHADER_STAGE_SPECIFIC_CONSTANTS; ++constantIndex) {
		if (shaderSpecificConstants[constantIndex] != NULL) {
			program.m_ShaderSpecificVertexConstants[constantIndex] = g_pGL->GetUniformLocation(
					programGLHandle, shaderSpecificConstants[constantIndex]);
		}
	}
	memset(shaderSpecificConstants, 0, sizeof(shaderSpecificConstants));
	pixelProvider->GetConstants(pixelStaticIndex, pixelDynamicIndex, shaderSpecificConstants);
	for (int constantIndex = 0; constantIndex < IShaderSourceProvider::MAX_SHADER_STAGE_SPECIFIC_CONSTANTS; ++constantIndex) {
		if (shaderSpecificConstants[constantIndex] != NULL) {
			program.m_ShaderSpecificPixelConstants[constantIndex] = g_pGL->GetUniformLocation(
					programGLHandle, shaderSpecificConstants[constantIndex]);
		}
	}

	// Sampler indices (first pixel, then vertex from the defined offset).
	// D0GTODO: Mark the new program as the one currently bound.
	g_pGL->UseProgram(programGLHandle);
	const char *samplerNames[CShaderAPIGLES2::MAX_SAMPLERS];
	memset(samplerNames, 0, sizeof(samplerNames));
	int pixelSamplerCount = pixelProvider->GetSamplers(pixelStaticIndex, pixelDynamicIndex, samplerNames);
	if (pixelSamplerCount > 0) {
		for (unsigned int samplerIndex = 0; samplerIndex < CShaderAPIGLES2::MAX_SAMPLERS; ++samplerIndex) {
			const char *samplerName = samplerNames[samplerIndex];
			if (samplerName != NULL) {
				int samplerLocation = g_pGL->GetUniformLocation(programGLHandle, samplerName);
				if (samplerLocation >= 0) {
					g_pGL->Uniform1i(samplerLocation, samplerIndex);
				}
			}
		}
	}
	memset(samplerNames, 0, sizeof(samplerNames));
	if (vertexProvider->GetSamplers(vertexStaticIndex, vertexDynamicIndex, samplerNames) > 0) {
		for (unsigned int samplerIndex = 0; samplerIndex < CShaderAPIGLES2::MAX_SAMPLERS; ++samplerIndex) {
			const char *samplerName = samplerNames[samplerIndex];
			if (samplerName != NULL) {
				int samplerLocation = g_pGL->GetUniformLocation(programGLHandle, samplerName);
				if (samplerLocation >= 0) {
					g_pGL->Uniform1i(samplerLocation, pixelSamplerCount + samplerIndex);
				}
			}
		}
	}
}

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
	AssertMsg(0, "Shaders must be bound as programs.");
}

void CShaderAPIGLES2::BindGeometryShader(GeometryShaderHandle_t hGeometryShader) {
	AssertMsg(0, "Shaders must be bound as programs.");
}

void CShaderAPIGLES2::BindPixelShader(PixelShaderHandle_t hPixelShader) {
	AssertMsg(0, "Shaders must be bound as programs.");
}
