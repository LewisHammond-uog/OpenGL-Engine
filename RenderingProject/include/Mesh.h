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
#include "Material.h"

//Forward Declare
struct aiScene; //AssImp Scene
struct aiMesh; //AssImp Mesh
class aiMaterial; //AssImp Material
enum aiTextureType; //AssImp Texture Type
class Texture; //Project Texture (Texture.h)
class WorldTransform; //Project Transform (WorldTransform.h)


class Mesh
{
public:
	Mesh();
	~Mesh();

	bool LoadMesh(const std::string& a_filePath);

	void Render(GLenum a_drawMode = GL_PATCHES);

	Material& GetMaterial();

	WorldTransform* m_transform;

private:

	bool InitFromScene(const aiScene* a_pScene, const std::string& a_filePath);
	void InitAllMeshes(const aiScene* a_pScene);
	void InitSingleMesh(const aiMesh* a_pMesh);
	bool InitMaterials(const aiScene* a_pScene, const std::string& a_filePath);

	bool LoadDiffuseTexture(const std::string& a_directory, const aiMaterial* a_assimpMaterial, int a_index);
	bool LoadSpecularTexture(const std::string& a_directory, const aiMaterial* a_assimpMaterial, int a_index);
	bool LoadNormalTexture(const std::string& a_directory, const aiMaterial* a_assimpMaterial, const int a_index);
	void LoadColours(const aiMaterial* a_pMaterial, int a_index);
	Texture* LoadTexture(const std::string& a_directory, const aiMaterial* a_assimpMaterial, const aiTextureType a_textureType);

	void PopulateOpenGLBuffers();

	void CountVertsAndIndices(const aiScene* pScene, unsigned int& a_numVerts, unsigned int& a_numIndices);

	void ReserveSpaceInCPUBuffers(const unsigned int a_numVerts, const unsigned int a_numIndices);

	std::string GetDirectoryFromPath(const std::string& a_filePath);

	//Transform of the model;

	unsigned int m_VAO; //Vertex Array Object

	enum BUFFER_TYPE
	{
		INDEX_BUFFER = 0, //Index
		POS_VB = 1, //Position
		TEXCOORD_VB = 2, //TexCoord
		NORMAL_VB = 3, //Normals
		TANGENT_VB = 4, //Tangents

		NUM_BUFFERS
	};

	//Buffers for mesh data
	GLuint m_buffers[NUM_BUFFERS] = { 0 };

	//Array of vertecies and textures
	std::vector<MeshEntry> m_meshes;
	std::vector<Material> m_materials;

	//Temp Sotrage Space for vertex infomation before being sent to the GPU
	std::vector<glm::vec3> m_positions;
	std::vector<glm::vec3> m_normals;
	std::vector<glm::vec2> m_texCoords;
	std::vector<glm::vec2> m_tangents;
	std::vector<unsigned int> m_indicies;

};



#endif //!__MESH_H__
