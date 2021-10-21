#ifndef __Lighting_FBX_Normal_H_
#define __Lighting_FBX_Normal_H_

#include "Application.h"
#include "FBXFile.h"
#include <glm/glm.hpp>

// Derived application class that wraps up all globals neatly
class Lighting_FBX_Normal : public Application
{
public:

	Lighting_FBX_Normal();
	virtual ~Lighting_FBX_Normal();

protected:

	virtual bool onCreate();
	virtual void Update(float a_deltaTime);
	virtual void Draw();
	virtual void Destroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
	glm::mat4	m_modelMatrix;

	unsigned int	m_programID;
	unsigned int	m_vertexShader;
	unsigned int	m_fragmentShader;

	unsigned int m_maxIndices;
	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;

	//Texture Handle for OpenGL
	unsigned int m_textureID;

	//FBX Model File
	FBXFile* m_fbxModel;
	

	glm::vec4	m_lightPos;
	
};

#endif // __Lighting_FBX_Normal_H_