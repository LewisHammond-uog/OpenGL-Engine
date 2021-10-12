#ifndef __Week02_EdgeDetection_H_
#define __Week02_EdgeDetection_H_

#include "Application.h"
#include <glm/glm.hpp>

// Derived application class that wraps up all globals neatly
class Week02_EdgeDetection : public Application
{
public:

	Week02_EdgeDetection();
	virtual ~Week02_EdgeDetection();

protected:

	virtual bool onCreate();
	virtual void Update(float a_deltaTime);
	virtual void Draw();
	virtual void Destroy();

	
	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	unsigned int m_shaderProgramID;
	unsigned int m_vertexShaderID;
	unsigned int m_fragmentShaderID;

	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;

	typedef struct Vertex
	{
		glm::vec4 position;
		glm::vec4 colour;
	}Vertex;
};

#endif // __Week02_EdgeDetection_H_