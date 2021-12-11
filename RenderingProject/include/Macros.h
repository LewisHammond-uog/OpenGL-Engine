//File containing helpful macros
#ifndef __MACROS_H__
#define __MACROS_H__

//Size of an array in elements is equal to the size of the array / the size of one of it's elements
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

//Macro for how to load the model for ASSIMP
//                      Make Everything Triangles| Smooth Normals             | Join Identical Verts-save memory| Calculate Tangents on import
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace)


//Defines for texture units used for rendering
//Colour
#define COLOUR_TEXTURE_UNIT GL_TEXTURE0
#define COLOUR_TEXTURE_INDEX 0
//Specular Map
#define SPECULAR_POWER_TEXTURE_UNIT GL_TEXTURE1
#define SPECULAR_POWER_TEXTURE_INDEX 1
//Normals
#define NORMAL_TEXTURE_UNIT GL_TEXTURE2
#define NORMAL_TEXTURE_INDEX 2
//Shadows
#define SHADOW_TEXTURE_UNIT GL_TEXTURE3
#define SHADOW_TEXTURE_INDEX 3

#endif
