#ifndef __GEOMETRY_PROGRAM_H__
#define __GEOMETRY_PROGRAM_H__

#include <glm/glm.hpp>

#include "ShaderProgram.h"


class GeometryProgram : public ShaderProgram
{
public:
	GeometryProgram() = default;

	bool Initialise() override;

	void SetupBuffers();

	void DrawParticles() const;

	void SetProjectionView(glm::mat4 a_projectionView) const;
	void SetCameraWorldPos(glm::vec4 a_cameraPos) const;

private:

	//Uniform Locations
	GLint m_projectionViewLocation;
	GLint m_cameraWorldPosLocation;
	GLint m_textureLocation;

	glm::vec4* m_vertices;
	const int m_particleCount = 4;

	unsigned int m_VAO; //Vertex Array Object
	unsigned int m_VBO; //Vertex Buffer Object
	unsigned int m_IBO; //Index Buffer Object

};

#endif //__GEOMETRY_PROGRAM_H__
