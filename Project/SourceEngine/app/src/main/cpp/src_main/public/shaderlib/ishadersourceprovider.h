// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#ifndef SHADERLIB_SOURCEPROVIDER_H
#define SHADERLIB_SOURCEPROVIDER_H

#include "tier0/platform.h"
#include "tier1/utlvector.h"

//-----------------------------------------------------------------------------
// Base interface for source providers for run-time-compiled shaders.
//-----------------------------------------------------------------------------
abstract_class IShaderSourceProvider {
public:
	enum ShaderStage_t {
		SHADER_STAGE_VERTEX,
		SHADER_STAGE_PIXEL,

		SHADER_STAGE_COUNT
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

	// Writes the shader source code (as strings that won't be destroyed when this method is exited) to the vector,
	// assuming the vector has already been cleared and contains the main header (for GLESSL - precision, in/out, etc.)
	virtual void GetSourceCode(int staticIndex, int dynamicIndex, CUtlVector<const char *> &source) const = 0;

protected:
	IShaderSourceProvider() {}
};

#endif
