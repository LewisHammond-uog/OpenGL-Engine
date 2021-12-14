#ifndef __RenderingProject_H_
#define __RenderingProject_H_

#include "Application.h"
#include <glm/glm.hpp>


//Forward declare
class Mesh;
class LightingProgram;
class LightingManager;
class DirectionalLight;
class ShadowProgram;
class ShadowFBO;
class WaterProgram;

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

	void WaterPass(); //Pass to Render Water
	void ShadowPass(glm::mat4 a_lightProjectionView); //Pass to Render Shadows
	void RenderPass(glm::mat4 a_lightProjectionView); //Pass to Render Standard Geometry

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
	glm::mat4	m_viewMatrix;

	unsigned int m_programID;

private:
	Mesh* m_pSceneMesh;
	Mesh* m_pWaterMesh;
	
	//Light used as the shadow source
	DirectionalLight* m_pDirecitonalSourceLight;

	LightingProgram* m_pLightingProgram;
	LightingManager* m_pLightingManager;

	ShadowProgram* m_pShadowProgram;
	ShadowFBO* m_pFBO;

	WaterProgram* m_pWaterProgram;
	
};

#endif // __RenderingProject_H_