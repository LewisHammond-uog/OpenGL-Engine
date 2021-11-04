#include "Mesh.h"

//Helpful Macros
#include "Macros.h"

//OpenGL Includes
#include "glad/glad.h"

//Assimp Includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//Glm Includes
#include <glm/glm.hpp>

//Framework Includes
#include "Application_Log.h"

//Application Includes
#include "Texture.h"

//Macros for defintions of locations of attributes within the shader code
#define SHADER_POSITION_LOC 0
#define SHADER_TEX_COORD_LOC 1
#define SHADER_NORMAL_LOC 2

bool Mesh::LoadMesh(const std::string& a_filePath)
{
	//Create the VAO
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	//Create buffers for vertex attributes
	glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_buffers), m_buffers);

	bool success = false;

	//Import Model in to an AssImp Scene
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(a_filePath.c_str(), ASSIMP_LOAD_FLAGS);

	//If the scene is invalid then there has been an error loading
	if(!pScene)
	{
		Application_Log* log = Application_Log::Get();
		if(log)
		{
			log->addLog(LOG_ERROR, "Could not load model %s: %s", a_filePath.c_str(), importer.GetErrorString());
		}
	}else
	{
		success = InitFromScene(pScene, a_filePath);
	}

	//Unbind VAO
	glBindVertexArray(0);
	return success;
}

void Mesh::Render()
{
	//Bind the VAO
	glBindVertexArray(m_VAO);

	for(unsigned int i = 0; i < m_meshes.size(); i++)
	{
		const unsigned int matIndex = m_meshes[i].MaterialIndex;

		//Bind texture if we have one
		if(m_textures[matIndex])
		{
			m_textures[matIndex]->Bind(GL_TEXTURE0); //todo remove magic number
		}

		//Do the draw!
		glDrawElementsBaseVertex(GL_TRIANGLES,
			m_meshes[i].NumIndices, //Number of indicies to draw
			GL_UNSIGNED_INT, //Index data type
			(void*)(sizeof(unsigned int) * m_meshes[i].BaseIndex), //Offset to the 1st index of the current mesh
			m_meshes[i].BaseVertex); //Base vertex from the current draw call
	}

	//Unbind VAO
	glBindVertexArray(0);

}

/// <summary>
/// Load a scene from AssImp in to a mesh
/// </summary>
/// <param name="a_pScene">Loaded AssImp Scene</param>
/// <param name="a_filePath">File path of the loaded scene</param>
/// <returns>If scene was successfully loaded</returns>
bool Mesh::InitFromScene(const aiScene* a_pScene, const std::string& a_filePath)
{
	//Early out for nullptr
	if(!a_pScene)
	{
		return false;
	}

	//Set the size of our arrays to load the scene
	m_meshes.resize(a_pScene->mNumMeshes);
	m_textures.resize(a_pScene->mNumMaterials);

	unsigned int numVerts = 0;
	unsigned int numIndices = 0;
	CountVertsAndIndices(a_pScene, numVerts, numIndices);

	ReserveSpaceInCPUBuffers(numVerts, numIndices);

	InitAllMeshes(a_pScene);

	InitMaterials(a_pScene, a_filePath);

	PopulateOpenGLBuffers();
	return true;
}

/// <summary>
/// Initalise all of the meshes in the scene to CPU memory
/// </summary>
/// <param name="a_pScene"></param>
void Mesh::InitAllMeshes(const aiScene* a_pScene)
{
	for (unsigned int i = 0; i < m_meshes.size(); ++i)
	{
		const aiMesh* mesh = a_pScene->mMeshes[i];
		InitSingleMesh(mesh);
	}
}

/// <summary>
/// Initalise a single AssImp mesh in to CPU Memory
/// </summary>
/// <param name="a_pMesh"></param>
void Mesh::InitSingleMesh(const aiMesh* a_pMesh)
{
	//Populate the vertex attribute buffers
	for(unsigned int i = 0; i < a_pMesh->mNumVertices; ++i)
	{
		const glm::vec3 pos = glm::vec3(a_pMesh->mVertices[i].x, a_pMesh->mVertices[i].y, a_pMesh->mVertices[i].z);
		const glm::vec3 normal = glm::vec3(a_pMesh->mNormals[i].x, a_pMesh->mNormals[i].y, a_pMesh->mNormals[i].z);
		const glm::vec2 texCoord = a_pMesh->HasTextureCoords(0) ? glm::vec2(a_pMesh->mTextureCoords[0][i].x, a_pMesh->mTextureCoords[0][i].y) : glm::vec2(0.f);

		m_positions.push_back(pos);
		m_normals.push_back(normal);
		m_texCoords.push_back(texCoord);
	}

	//Populate the index buffer
	for(unsigned int i = 0; i < a_pMesh->mNumFaces; i++)
	{
		const aiFace& face = a_pMesh->mFaces[i];
		m_indicies.push_back(face.mIndices[0]);
		m_indicies.push_back(face.mIndices[1]);
		m_indicies.push_back(face.mIndices[2]);
	}
}

/// <summary>
/// Initalise the materials in the scene
/// </summary>
/// <param name="a_pScene"></param>
/// <param name="a_filePath"></param>
/// <returns></returns>
bool Mesh::InitMaterials(const aiScene* a_pScene, const std::string& a_filePath)
{
	//Textures are stored in the same direcory - extract the directory from the file path
	std::string::size_type slashIndex = a_filePath.find_last_of("/");
	std::string directory;

	if(slashIndex == std::string::npos)
	{
		directory = ".";
	}else if(slashIndex == 0)
	{
		directory = "/";
	}else
	{
		directory = a_filePath.substr(0, slashIndex);
	}

	bool success = true;

	//Init the materials
	for(unsigned int i = 0; i < a_pScene->mNumMaterials; i++)
	{
		const aiMaterial* pMaterial = a_pScene->mMaterials[i];
		m_textures[i] = nullptr;

		//Check if we have a diffuse texture
		if(pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			
			//Load the texture in to path
			aiString path;
			if(pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			{
				//construct the full path of the texutre file
				std::string p = path.data;
				if(p.substr(0,2) == ".\\")
				{
					p = p.substr(2, p.size() - 2);
				}
				std::string fullFilePath = directory + "/" + p;

				//Load textures
				m_textures[i] = new Texture(GL_TEXTURE_2D, fullFilePath.c_str());
				if(!m_textures[i]->Load())
				{
					//If failed to load then delete and exit out
					delete m_textures[i];
					m_textures[i] = nullptr;
					success = false;
				}
			}
		}
	}

	return success;

}

/// <summary>
/// Count the number of verticies and indices within the scene
/// </summary>
/// <param name="pScene">Scene to count</param>
/// <param name="a_numVerts"></param>
/// <param name="a_numIndices"></param>
void Mesh::CountVertsAndIndices(const aiScene* pScene, unsigned& a_numVerts, unsigned& a_numIndices)
{
	//Foreach mesh in the scene
	for(unsigned int i = 0; i < m_meshes.size(); ++i)
	{
		const aiMesh* currentMeshInScene = pScene->mMeshes[i];

		m_meshes[i].MaterialIndex = currentMeshInScene->mMaterialIndex; //Set the material index of this mesh
		m_meshes[i].NumIndices = currentMeshInScene->mNumFaces * 3; //Number of indicies is faces *3 because we know everything is a triangle
		m_meshes[i].BaseVertex = a_numVerts; //Index of the first vertex of the current mesh
		m_meshes[i].BaseIndex = a_numIndices; //Index of the first index of the current mesh

		//Increase the number of verts and indicies ready for the next mesh to process
		a_numVerts += currentMeshInScene->mNumVertices;
		a_numIndices += m_meshes[i].NumIndices;
	}
}

/// <summary>
/// Reserve Space in the CPU side storage for our buffers 
/// </summary>
/// <param name="a_numVerts">Number of Verts in the scene</param>
/// <param name="a_numIndices">Number of Indicies in the scene</param>
void Mesh::ReserveSpaceInCPUBuffers(const unsigned a_numVerts, const unsigned a_numIndices)
{
	m_positions.reserve(a_numVerts);
	m_normals.reserve(a_numVerts);
	m_texCoords.reserve(a_numVerts);
	m_indicies.reserve(a_numIndices);
}

/// <summary>
/// Populate Open GL buffers with all of the vertex infomation from this object
/// </summary>
void Mesh::PopulateOpenGLBuffers()
{
	//Setup Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[POS_VB]); //Bind Buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_positions[0]) * m_positions.size(), &m_positions[0], GL_STATIC_DRAW); //Send Data
	glEnableVertexAttribArray(SHADER_POSITION_LOC); //Enable vertex attrib
	glVertexAttribPointer(SHADER_POSITION_LOC, 3, GL_FLOAT, GL_FALSE, 0, 0); //Set pointer to data but because the positions are packed we don't need to give a stride

	//TexCoord Buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[TEXCOORD_VB]); //Bind Buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_texCoords[0]) * m_texCoords.size(), &m_texCoords[0], GL_STATIC_DRAW); //Send Data
	glEnableVertexAttribArray(SHADER_TEX_COORD_LOC); //Enable vertex attrib
	glVertexAttribPointer(SHADER_TEX_COORD_LOC, 3, GL_FLOAT, GL_FALSE, 0, 0); //Set pointer to data but because the positions are packed we don't need to give a stride

	//Normal Buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[NORMAL_VB]); //Bind Buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_normals[0]) * m_normals.size(), &m_normals[0], GL_STATIC_DRAW); //Send Data
	glEnableVertexAttribArray(SHADER_NORMAL_LOC); //Enable vertex attrib
	glVertexAttribPointer(SHADER_NORMAL_LOC, 3, GL_FLOAT, GL_FALSE, 0, 0); //Set pointer to data but because the positions are packed we don't need to give a stride

	//Set the element buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indicies[0]) * m_indicies.size(), &m_indicies[0], GL_STATIC_DRAW);
}