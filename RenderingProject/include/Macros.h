//File containing helpful macros
#ifndef __MACROS_H__
#define __MACROS_H__

//Size of an array in elements is equal to the size of the array / the size of one of it's elements
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

//Macro for how to load the model for ASSIMP
//                      Make Everything Triangles| Smooth Normals             | Join Identical Verts - save memory
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices)

#endif
