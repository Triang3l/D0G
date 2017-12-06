// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#ifndef SHADERLIB_SOURCEPROVIDER_H
#define SHADERLIB_SOURCEPROVIDER_H

#include "tier0/platform.h"
#include "tier1/utlvector.h"

//-----------------------------------------------------------------------------
// Base interface for source code providers for run-time-compiled shaders.
//-----------------------------------------------------------------------------
abstract_class IShaderSourceProvider {
public:
	enum ShaderStage_t {
		SHADER_STAGE_VERTEX,
		SHADER_STAGE_PIXEL,

		SHADER_STAGE_COUNT
	};

	enum {
		MAX_SHADER_STAGE_SPECIFIC_CONSTANTS = 32
	};

	// Returns which shader stage the source code is provided for.
	// Different stages have separate provider namespaces.
	virtual ShaderStage_t GetStage() const = 0;

	// Unique identifier (within the stage) for the provider dictionary.
	virtual const char *GetName() const = 0;

	// The version of the source for caching compiled shaders.
	virtual int GetVersion() const = 0;

	// Writes the dynamic combo indices that need to be precompiled for the static combo.
	virtual void GetDynamicCombos(int staticIndex, CUtlVector<int> &indices) const {
		indices.AddToTail(0);
	}

	// Writes the shader source code (as strings that won't be destroyed when
	// this method is exited) to the vector, assuming the vector has already
	// been cleared and contains the main header (for GLESSL - precision,
	// attribute/varying or in/out, etc.)
	virtual void GetSource(int staticIndex, int dynamicIndex, CUtlVector<const char *> &source) const = 0;

	// Writes the names of shader stage-specific (not standard) constants to
	// the array, with the indices the same as used in Set<Stage>ShaderConstant
	// calls, or NULL for the constants not used in the specified combo.
	// Generally at most MAX_SHADER_STAGE_SPECIFIC_CONSTANTS constants.
	virtual void GetConstants(int staticIndex, int dynamicIndex, const char **constantNames) const {}

	// Writes the names of samplers to the array, with the indices the same as
	// sampler index in BindTexture calls, or NULL for the samplers not used in
	// the specified combo. Maximum number of samplers specified by Shader API.
	virtual void GetSamplers(int staticIndex, int dynamicIndex, const char **samplerNames) const {}

protected:
	IShaderSourceProvider() {}
};

#endif
