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

	void DrawDepthBufferWindow();
	void DrawSceneSettingsWindow();

	//If the scene should be drawn in wire frame mode
	bool m_bDrawWireFrame{};
	//If we should use vsync
	bool m_bVsyncEnabled{};
	//If we should use MSAA
	bool m_bAAEnabled;

	//Meshes used for scene rendering
	Mesh* m_pSceneMesh;
	Mesh* m_pWaterMesh;
	
	//Light used as the shadow source
	DirectionalLight* m_pDirecitonalSourceLight;

	//Lighting Management
	LightingProgram* m_pLightingProgram;
	LightingManager* m_pLightingManager;

	//Shadows
	ShadowProgram* m_pShadowProgram;
	int m_iShadowMapWidth{};
	int m_iShadowMapHeight{};
	ShadowFBO* m_pFBO;

	//Water
	WaterProgram* m_pWaterProgram;
	
};

#endif // __RenderingProject_H_