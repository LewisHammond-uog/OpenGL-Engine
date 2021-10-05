#ifndef __Week_1___VAOs_H_
#define __Week_1___VAOs_H_

#include "Application.h"
#include <glm/glm.hpp>

// Derived application class that wraps up all globals neatly
class Week_1___VAOs : public Application
{
public:

	Week_1___VAOs();
	virtual ~Week_1___VAOs();

protected:

	virtual bool onCreate();
	virtual void Update(float a_deltaTime);
	virtual void Draw();
	virtual void Destroy();

	struct Vertex
	{
		glm::vec4 position;
		glm::vec4 colour;
	};

	Vertex* m_vertices;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	unsigned int m_programID;
	unsigned int m_vertexShaderID;
	unsigned int m_fragmentShaderID;

	unsigned int m_maxVerts;

	unsigned int m_vao; //Vertex Array Object is used as a wrapper to keep buffers between frames
	unsigned int m_vbo; //Vertex Buffer Object is a buffer with the location of all of the vertexes of the object
	unsigned int m_ibo; //Index Buffer Object is a buffer with the directions to 'join the dots' of vertexes to draw the object

};

#endif // __Week_1___VAOs_H_