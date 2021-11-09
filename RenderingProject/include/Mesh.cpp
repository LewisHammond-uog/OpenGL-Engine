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
#include "Material.h"
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
		if (m_materials[matIndex].m_pDiffuseTexture)
		{
			m_materials[matIndex].m_pDiffuseTexture->Bind(COLOUR_TEXTURE_UNIT); //todo remove magic number
		}

		//Bind a specular texture if we have one
		if (m_materials[matIndex].m_pSpecularTexture)
		{
			m_materials[matIndex].m_pSpecularTexture->Bind(SPECULAR_POWER_TEXTURE_UNIT); //todo remove magic number
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
/// Get the material for the mesh
/// </summary>
/// <returns></returns>
Material& Mesh::GetMaterial()
{
	for(unsigned int i = 0; i < m_materials.size(); i++)
	{
		if(m_materials[i].m_ambientColour != glm::vec3(0,0,0))
		{
			return m_materials[i];
		}
	}
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
	m_materials.resize(a_pScene->mNumMaterials);

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
	bool success = true;
	const std::string directory = GetDirectoryFromPath(a_filePath);

	//Init the materials
	for(unsigned int i = 0; i < a_pScene->mNumMaterials; i++)
	{
		const aiMaterial* pMaterial = a_pScene->mMaterials[i];

		if(!LoadDiffuseTexture(directory, pMaterial, static_cast<int>(i)))
		{
			return false;
		}

		LoadSpecularTexture(directory, pMaterial, static_cast<int>(i));


		//Get if the Material has a ambient lighting attribute
		aiColor3D ambientColour = aiColor3D(0.0f, 0.0f, 0.0f);
		if(pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambientColour) == AI_SUCCESS)
		{
			m_materials[i].m_ambientColour.r = ambientColour.r;
			m_materials[i].m_ambientColour.g = ambientColour.g;
			m_materials[i].m_ambientColour.b = ambientColour.b;
		}

		//Get if the Material has a diffuse colour attribute
		aiColor3D diffuseColour = aiColor3D(0.0f, 0.0f, 0.0f);
		if (pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColour) == AI_SUCCESS)
		{
			m_materials[i].m_diffuseColour.r = diffuseColour.r;
			m_materials[i].m_diffuseColour.g = diffuseColour.g;
			m_materials[i].m_diffuseColour.b = diffuseColour.b;
		}

	}

	return success;

}

/// <summary>
/// Load a diffuse texture from the diretory and save it in to material
/// </summary>
/// <param name="a_directory">Directory of the Textures</param>
/// <param name="a_assimpMaterial">Assimp Material to read textures from</param>
/// <param name="a_index">Index of Material to Load</param>
bool Mesh::LoadDiffuseTexture(const std::string& a_directory, const aiMaterial* a_assimpMaterial, const int a_index)
{
	m_materials[a_index].m_pDiffuseTexture = LoadTexture(a_directory, a_assimpMaterial, aiTextureType_DIFFUSE);
	return m_materials[a_index].m_pDiffuseTexture != nullptr ? true : false;
}

bool Mesh::LoadSpecularTexture(std::string a_directory, const aiMaterial* a_assimpMaterial, int a_index)
{
	m_materials[a_index].m_pSpecularTexture = LoadTexture(a_directory, a_assimpMaterial, aiTextureType_SHININESS);
	return m_materials[a_index].m_pSpecularTexture != nullptr ? true : false;
}

/// <summary>
/// Load colours (for lighting) from the AssImp Material 
/// </summary>
/// <param name="a_pMaterial"></param>
/// <param name="a_index"></param>
void Mesh::LoadColours(const aiMaterial* a_pMaterial, const int a_index)
{
	//Get if the Material has a ambient lighting attribute
	aiColor3D ambientColour = aiColor3D(0.0f, 0.0f, 0.0f);
	if (a_pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambientColour) == AI_SUCCESS)
	{
		m_materials[a_index].m_ambientColour.r = ambientColour.r;
		m_materials[a_index].m_ambientColour.g = ambientColour.g;
		m_materials[a_index].m_ambientColour.b = ambientColour.b;
	}

	//Get if the Material has a diffuse colour attribute
	aiColor3D diffuseColour = aiColor3D(0.0f, 0.0f, 0.0f);
	if (a_pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColour) == AI_SUCCESS)
	{
		m_materials[a_index].m_diffuseColour.r = diffuseColour.r;
		m_materials[a_index].m_diffuseColour.g = diffuseColour.g;
		m_materials[a_index].m_diffuseColour.b = diffuseColour.b;
	}

	//Get if the Material has a specular colour attribute
	aiColor3D specularColour = aiColor3D(0.0f, 0.0f, 0.0f);
	if (a_pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specularColour) == AI_SUCCESS)
	{
		m_materials[a_index].m_specularColour.r = specularColour.r;
		m_materials[a_index].m_specularColour.g = specularColour.g;
		m_materials[a_index].m_specularColour.b = specularColour.b;
	}

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
/// Get the directory from a given file path
/// </summary>
/// <param name="a_filePath">File path</param>
/// <returns></returns>
std::string Mesh::GetDirectoryFromPath(const std::string& a_filePath)
{
	//Textures are stored in the same direcory - extract the directory from the file path
	std::string::size_type slashIndex = a_filePath.find_last_of("/");
	std::string directory;

	if (slashIndex == std::string::npos)
	{
		directory = ".";
	}
	else if (slashIndex == 0)
	{
		directory = "/";
	}
	else
	{
		directory = a_filePath.substr(0, slashIndex);
	}

	return directory;
}

/// <summary>
/// Load a texture a given texture type from a directory and material
/// </summary>
/// <param name="a_directory">Directory of the Texture</param>
/// <param name="a_assimpMaterial">Assimp Material to load texture from</param>
/// <param name="a_textureType">Type of Texture to load</param>
/// <returns></returns>
Texture* Mesh::LoadTexture(const std::string& a_directory, const aiMaterial* a_assimpMaterial,
	const aiTextureType a_textureType)
{
	//Texture to return, default to nullptr if we cannot load one
	Texture* retTexture = nullptr;

	//Check if we have a diffuse texture
	if (a_assimpMaterial->GetTextureCount(a_textureType) > 0)
	{

		//Load the texture in to path
		aiString path;
		if (a_assimpMaterial->GetTexture(a_textureType, 0, &path) == AI_SUCCESS)
		{
			//construct the full path of the texutre file
			std::string p = path.data;
			if (p.substr(0, 2) == ".\\")
			{
				p = p.substr(2, p.size() - 2);
			}

			const std::string fullFilePath = a_directory + "/" + p;

			//Load textures
			retTexture = new Texture(GL_TEXTURE_2D, fullFilePath);
			if (!retTexture->Load())
			{
				//If failed to load then delete and exit out
				delete retTexture;
				retTexture = nullptr;
			}
		}
	}

	return retTexture;
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
	glVertexAttribPointer(SHADER_TEX_COORD_LOC, 2, GL_FLOAT, GL_FALSE, 0, 0); //Set pointer to data but because the positions are packed we don't need to give a stride

	//Normal Buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[NORMAL_VB]); //Bind Buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_normals[0]) * m_normals.size(), &m_normals[0], GL_STATIC_DRAW); //Send Data
	glEnableVertexAttribArray(SHADER_NORMAL_LOC); //Enable vertex attrib
	glVertexAttribPointer(SHADER_NORMAL_LOC, 3, GL_FLOAT, GL_FALSE, 0, 0); //Set pointer to data but because the positions are packed we don't need to give a stride

	//Set the element buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indicies[0]) * m_indicies.size(), &m_indicies[0], GL_STATIC_DRAW);
}