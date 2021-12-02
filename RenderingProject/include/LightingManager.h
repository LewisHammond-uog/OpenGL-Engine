#ifndef __LIGHTING_MANAGER_H__
#define __LIGHTING_MANAGER_H__

#include "LightingProgram.h"
#include "SpotLight.h"
#include "PointLight.h"

//Forward Declarations
class WorldTransform;

class LightingManager
{
public:
	explicit LightingManager(LightingProgram* a_program);
	~LightingManager();

	void Update(WorldTransform& a_pWorldTransform);

	DirectionalLight* CreateDirectionalLight();

	PointLight* CreatePointLight(glm::vec3 a_worldPosition = glm::vec3(0.f), 
	                             glm::vec3 a_colour = glm::vec3(1.f, 1.f, 1.f),
	                             float a_ambientIntensity = 1.0f,
	                             float a_diffuseIntensity = 1.0f,
	                             float a_linearAttenuation = 0.1f,
	                             float a_exponentialAttenuation = 0.1f);

	SpotLight* CreateSpotLight(glm::vec3 a_worldPosition = glm::vec3(0.f),
								glm::vec3 a_worldDirection = glm::vec3(0.f, 1.0f, 0.0f), //down
								glm::vec3 a_colour = glm::vec3(1.f, 1.f, 1.f),
								float a_diffuseIntensity = 1.0f,
								float a_linearAttenuation = 0.1f,
								float a_exponentialAttenuation = 0.1f,
								float a_cutOffAngle = 20.f);

	void RenderImguiWindow();

	//All point lights
	PointLight* m_pointLights[MAX_POINT_LIGHTS]{};
	unsigned int m_createdPointLightsCount = 0;

private:

	void DrawImguiDirectionalLightSetting(DirectionalLight* a_pLight) const;
	void DrawImguiPointLightSetting(PointLight* a_pLight) const;
	void DrawImguiSpotLightSetting(SpotLight* a_pLight) const;

	//Lighting Program
	LightingProgram* m_pLightingProgram;

	//Single Directional Light
	DirectionalLight* m_pDirectionalLight;

	//All point lights
	//PointLight* m_pointLights[MAX_POINT_LIGHTS]{};
	//unsigned int m_createdPointLightsCount = 0;

	//All spot lights
	SpotLight* m_spotLights[MAX_SPOT_LIGHTS]{};
	unsigned int m_createdSpotLightCount = 0;

};

#endif // __LIGHTING_MANAGER_H__
