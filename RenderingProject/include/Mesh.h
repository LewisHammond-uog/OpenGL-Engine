#ifndef __MESH_H__
#define __MESH_H__

//C++ Includes
#include <string>
#include <vector>

//OpenGL Includes
#include "glad/glad.h"

//GLM Includes
#include <glm/detail/type_vec.hpp>

//Project Includes
#include "MeshEntry.h"

//Forward Declare
struct aiScene; //AssImp Scene
struct aiMesh; //AssImp Mesh
class Texture; //Project Texture (Texture.h)

class Mesh
{
public:
	Mesh() = default;
	~Mesh() = default;

	bool LoadMesh(const std::string& a_filePath);

	void Render();

private:

	bool InitFromScene(const aiScene* a_pScene, const std::string& a_filePath);
	void InitAllMeshes(const aiScene* a_pScene);
	void InitSingleMesh(const aiMesh* a_pMesh);
	bool InitMaterials(const aiScene* a_pScene, const std::string& a_filePath);

	void PopulateOpenGLBuffers();

	void CountVertsAndIndices(const aiScene* pScene, unsigned int& a_numVerts, unsigned int& a_numIndices);

	void ReserveSpaceInCPUBuffers(const unsigned int a_numVerts, const unsigned int a_numIndices);

	unsigned int m_VAO; //Vertex Array Object

	enum BUFFER_TYPE
	{
		INDEX_BUFFER = 0, //Index
		POS_VB = 1, //Position
		TEXCOORD_VB = 2, //TexCoord
		NORMAL_VB = 3, //Normals

		NUM_BUFFERS
	};

	//Buffers for mesh data
	GLuint m_buffers[NUM_BUFFERS] = { 0 };

	//Array of vertecies and textures
	std::vector<MeshEntry> m_meshes;
	std::vector<Texture*> m_textures;

	//Temp Sotrage Space for vertex infomation before being sent to the GPU
	std::vector<glm::vec3> m_positions;
	std::vector<glm::vec3> m_normals;
	std::vector<glm::vec2> m_texCoords;
	std::vector<unsigned int> m_indicies;

};



#endif //!__MESH_H__
