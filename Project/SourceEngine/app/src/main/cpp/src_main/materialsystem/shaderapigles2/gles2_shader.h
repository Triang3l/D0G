// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#ifndef SHADERAPI_GLES2_SHADER_H
#define SHADERAPI_GLES2_SHADER_H

#include "gles2_glfuncs.h"
#include "shaderlib/ishadersourceprovider.h"
#include "tier0/platform.h"
#include "tier1/checksum_crc.h"
#include "tier1/UtlSortVector.h"
#include "tier1/utlvector.h"

typedef int ShaderProgramHandle_t;
#define SHADER_PROGRAM_HANDLE_INVALID ((ShaderProgramHandle_t) -1)

typedef int ShaderProgramStaticComboHandle_t;
#define SHADER_PROGRAM_STATIC_COMBO_HANDLE_INVALID ((ShaderProgramStaticComboHandle_t) -1)

enum StandardConstantIndex_t {
	STANDARD_CONSTANT_MODELVIEWPROJ,
	STANDARD_CONSTANT_VIEWPROJ,
	STANDARD_CONSTANT_VIEWMODEL,

	STANDARD_CONSTANT_COUNT
};

class CShaderManager {
public:
	CShaderManager();

	ShaderProgramStaticComboHandle_t GetVertexPixelProgramStaticCombo(
			const char *vertexProviderName, int vertexStaticIndex,
			const char *pixelProviderName, int pixelStaticIndex);

private:
	// Currently loaded programs.

	struct ShaderProgram_t {
		int m_VertexDynamicIndex;
		int m_PixelDynamicIndex;

		GLuint m_GLProgram;

		GLuint m_GLVertexShader;
		GLuint m_GLPixelShader;

		struct StandardConstant_t {
			int m_Location;
			uint64 m_LastUpdate;
		};

		StandardConstant_t m_StandardConstants[STANDARD_CONSTANT_COUNT];
		int m_ShaderSpecificVertexConstants[IShaderSourceProvider::MAX_SHADER_STAGE_SPECIFIC_CONSTANTS];
		int m_ShaderSpecificPixelConstants[IShaderSourceProvider::MAX_SHADER_STAGE_SPECIFIC_CONSTANTS];
	};

	struct ShaderProgramStaticCombo_t {
		const IShaderSourceProvider *m_VertexProvider;
		const IShaderSourceProvider *m_PixelProvider;

		int m_VertexStaticIndex;
		int m_PixelStaticIndex;

		ShaderProgramHandle_t m_FirstDynamicCombo;

		int m_VertexDynamicComboCount;
		int m_PixelDynamicComboCount;
	};

	struct ShaderProgramStaticComboDictEntry_t {
		CRC32_t m_Checksum;
		ShaderProgramStaticComboHandle_t m_Combo;
	};

	class ShaderProgramStaticComboLessFunc {
	public:
		bool Less(const ShaderProgramStaticComboDictEntry_t &entry1,
				const ShaderProgramStaticComboDictEntry_t &entry2, void *context);
	};

	CUtlVector<ShaderProgram_t> m_ShaderPrograms;

	CUtlVector<ShaderProgramStaticCombo_t> m_ShaderProgramStaticCombos;
	// D0GTODO: Use CUtlDict?
	CUtlSortVector<ShaderProgramStaticComboHandle_t, ShaderProgramStaticComboLessFunc> m_ShaderProgramStaticComboDict;

	// Program creation.

	static void DynamicIndexComparison(const int *combo1, const int *combo2);

	CUtlVector<int> m_NewProgramVertexDynamicIndices, m_NewProgramPixelDynamicIndices;
	CUtlVector<const char *> m_NewProgramVertexShaderSource, m_NewProgramPixelShaderSource;

	// Methods.

	GLuint CompileShader(const IShaderSourceProvider *provider, int staticIndex, int dynamicIndex);

	bool CompileVertexPixelProgram(ShaderProgramHandle_t handle,
			const IShaderSourceProvider *vertexProvider, int vertexStaticIndex, int vertexDynamicIndex,
			const IShaderSourceProvider *pixelProvider, int pixelStaticIndex, int pixelDynamicIndex);
};

extern CShaderManager *g_pShaderManager;

#endif
