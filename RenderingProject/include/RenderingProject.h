#ifndef __RenderingProject_H_
#define __RenderingProject_H_

#include "Application.h"
#include <glm/glm.hpp>


//Forward declare mesh
class Mesh;
class LightingProgram;
class LightingManager;
class ShadowProgram;
class ShadowFBO;

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

	void ShadowPass();
	void RenderPass(glm::mat4 a_viewMatrix);

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	unsigned int m_programID;

private:
	Mesh* pMesh;

	LightingProgram* pLightingProgram;
	LightingManager* pLightingManager;

	ShadowProgram* pShadowProgram;
	ShadowFBO* pFBO;
	
};

#endif // __RenderingProject_H_