#ifndef __Week02_EdgeDetection_H_
#define __Week02_EdgeDetection_H_

#include <string>

#include "Application.h"
#include <glm/glm.hpp>

// Derived application class that wraps up all globals neatly
class Week02_EdgeDetection : public Application
{
public:

	Week02_EdgeDetection();
	virtual ~Week02_EdgeDetection();

protected:

	bool LoadImageFromFile(std::string a_filePath, unsigned int& a_textureID, int& a_width, int& a_height);

	virtual bool onCreate();
	virtual void Update(float a_deltaTime);
	virtual void Draw();
	virtual void Destroy();

	unsigned int m_textureID; //handle to our texture
	int m_imageWidth;
	int m_imageHeight;

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
		glm::vec2 uvs; //texture coordinate
	}Vertex;
};

#endif // __Week02_EdgeDetection_H_