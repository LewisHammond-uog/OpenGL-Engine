#ifndef __RenderingProject_H_
#define __RenderingProject_H_

#include "Application.h"
#include <glm/glm.hpp>


//Forward declare mesh
class Mesh;
class LightingProgram;
class DirectionalLight;
class PointLight;

// Derived application class that wraps up all globals neatly
class RenderingProject : public Application
{
public:

	RenderingProject();
	virtual ~RenderingProject();

protected:

	virtual bool onCreate();
	virtual void Update(float a_deltaTime);
	virtual void Draw();
	virtual void Destroy();

	
	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	unsigned int m_programID;

	Mesh* pMesh;
	LightingProgram* pLightingProgram;
	//DirectionalLight* directionalLight;
	PointLight* pointLights[2];
};

#endif // __RenderingProject_H_