#ifndef __MESH_ENTRY_H__
#define __MESH_ENTRY_H__

//Data structure used for the creation
struct MeshEntry {
    MeshEntry()
    {
        m_uiNumIndices = 0;
        m_uiBaseVertex = 0;
        m_uiVBaseIndex = 0;
        m_uiMaterialIndex = -1;
    }

    unsigned int m_uiNumIndices;
    unsigned int m_uiBaseVertex;
    unsigned int m_uiVBaseIndex;
    unsigned int m_uiMaterialIndex;
};



#endif // __MESHENTRY_H__
