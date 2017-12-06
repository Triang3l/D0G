// Copyright Valve Corporation, All rights reserved.
// D0G modifications by Triang3l, derivative work, in public domain if detached from Valve's work.

#ifndef SHADERAPI_GLES2_MESH_H
#define SHADERAPI_GLES2_MESH_H

#include "materialsystem/imesh.h"

// Hardware morphing requires 2 additional attributes, and may be unavailable on some GLES2 devices.
// GLES2 defines the minimum number of vertex attributes as 8.
// However, GLES2 hardware with enough vertex attributes, or GLES3, may have hardware morphing.
// If hardware morphing is much faster than stream offset morphing, it may be enabled for specific devices.
// Update CHardwareConfig::HasFastVertexTextures if it's enabled and fully implemented.
// #define SHADERAPI_GLES2_HARDWARE_MORPH

// Not the actual attribute indices in shaders - rather, indices of indices.
// Bridges the engine meshes and the shader attributes,
// unifying different sizes/formats and removing unused attributes (like separate tangents).
enum VertexGLES2Attribute_t {
	VERTEX_GLES2_ATTRIBUTE_POSITION,
	VERTEX_GLES2_ATTRIBUTE_NORMAL,
	VERTEX_GLES2_ATTRIBUTE_TANGENT, // "User data" in the engine. Cross product sign in W.

	VERTEX_GLES2_ATTRIBUTE_BONEINDEX,
	VERTEX_GLES2_ATTRIBUTE_BONEWEIGHT,

	// Generally mutually exclusive. Color is for unlit materials, specular is for vertex-lit.
	VERTEX_GLES2_ATTRIBUTE_COLOR,
	VERTEX_GLES2_ATTRIBUTE_SPECULAR,

	VERTEX_GLES2_ATTRIBUTE_TEXCOORD_FIRST,
	VERTEX_GLES2_ATTRIBUTE_TEXCOORD_LAST =
			VERTEX_GLES2_ATTRIBUTE_TEXCOORD_FIRST + VERTEX_MAX_TEXTURE_COORDINATES - 1,

#ifdef SHADERAPI_GLES2_HARDWARE_MORPH
	VERTEX_GLES2_ATTRIBUTE_FLEXPOSITION, // Wrinkle in W.
	VERTEX_GLES2_ATTRIBUTE_FLEXNORMAL,
#endif

	VERTEX_GLES2_ATTRIBUTE_COUNT
};

#endif
