#ifndef __RenderingProject_H_
#define __RenderingProject_H_

#include "Application.h"
#include <glm/glm.hpp>


//Forward declare
class Mesh;
class LightingProgram;
class GeometryProgram;
class LightingManager;
class DirectionalLight;
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

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	unsigned int m_programID;

private:
	Mesh* pMesh;
	
	//Light used as the shadow source
	DirectionalLight* m_pShadowSourceLight;

	LightingProgram* pLightingProgram;
	LightingManager* pLightingManager;

	//Shadows
	ShadowProgram* pShadowProgram;
	ShadowFBO* pFBO;

	//Geometry Shader
	GeometryProgram* m_pGeoProgram;
};

#endif // __RenderingProject_H_