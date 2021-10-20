#ifndef __Week03_Lighting_H_
#define __Week03_Lighting_H_

#include "Application.h"
#include <glm/glm.hpp>
#include <FBXFile.h>

// Derived application class that wraps up all globals neatly
class Week03_Lighting : public Application
{
public:

	Week03_Lighting();
	virtual ~Week03_Lighting();

protected:

	virtual bool onCreate();
	virtual void Update(float a_deltaTime);
	virtual void Draw();
	virtual void Destroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;


	//FBX Model
	FBXFile* m_fbxModel;

	//Shader Programs
	unsigned int m_programID;
	unsigned int m_vertexShaderID;
	unsigned int m_fragmentShaderID;

	//VAO, VBO, IBO
	unsigned int m_vao;
	unsigned int m_modelBuffer[2];

	glm::mat4	m_modelMatrix; //Model Matrix 

	glm::vec4 m_lightPos; //Light position
};

#endif // __Week03_Lighting_H_