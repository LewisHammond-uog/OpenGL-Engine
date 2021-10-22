#ifndef __Shadows_H_
#define __Shadows_H_

#include "Application.h"
#include <glm/glm.hpp>
#include "FBXFile.h"

// Derived application class that wraps up all globals neatly
class Reflection : public Application
{
public:

	Reflection();
	virtual ~Reflection();

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

	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;

	//Frame Buffer objects
	unsigned int m_FBO;
	unsigned int m_FBO_texture; //The texture to output from the previous render
	unsigned int m_FBO_depth_texture; //The depth texture output from the previous pipline stage


	//FBX Model File
	FBXFile* m_fbxModel;

	glm::vec4 m_lightPos;
};

#endif // __Application_H_