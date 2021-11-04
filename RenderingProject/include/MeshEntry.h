#ifndef __MESH_ENTRY_H__
#define __MESH_ENTRY_H__

//Data structure used for the creation
struct MeshEntry {
    MeshEntry()
    {
        NumIndices = 0;
        BaseVertex = 0;
        BaseIndex = 0;
        MaterialIndex = -1;
    }

    unsigned int NumIndices;
    unsigned int BaseVertex;
    unsigned int BaseIndex;
    unsigned int MaterialIndex;
};



#endif // __MESHENTRY_H__
