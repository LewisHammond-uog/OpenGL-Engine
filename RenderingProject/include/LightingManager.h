#ifndef __LIGHTING_MANAGER_H__
#define __LIGHTING_MANAGER_H__

#include "LightingProgram.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "PointLight.h"

//Forward Declarations
class WorldTransform;

class LightingManager
{
public:
	explicit LightingManager(LightingProgram* a_pProgram);
	~LightingManager();

	void Update(WorldTransform& a_rWorldTransform);

	DirectionalLight* CreateDirectionalLight();

	PointLight* CreatePointLight(glm::vec3 a_v3WorldPosition = glm::vec3(0.f), 
	                             glm::vec3 a_v3Colour = glm::vec3(1.f, 1.f, 1.f),
	                             float a_fAmbientIntensity = 1.0f,
	                             float a_fDiffuseIntensity = 1.0f,
	                             float a_fLinearAttenuation = 0.1f,
	                             float a_fExponentialAttenuation = 0.1f);

	SpotLight* CreateSpotLight(glm::vec3 a_v3WorldPosition = glm::vec3(0.f),
								glm::vec3 a_v3WorldDirection = glm::vec3(0.f, -1.0f, 0.0f), //down
								glm::vec3 a_v3Colour = glm::vec3(1.f, 1.f, 1.f),
								float a_fDiffuseIntensity = 1.0f,
								float a_fLinearAttenuation = 0.1f,
								float a_fExponentialAttenuation = 0.1f,
								float a_fCutOffAngle = 20.f);

	void RenderImguiWindow();


private:

	void DrawImguiDirectionalLightSetting(DirectionalLight* a_pLight) const;
	void DrawImguiPointLightSetting(PointLight* a_pLight) const;
	void DrawImguiSpotLightSetting(SpotLight* a_pLight) const;

	//Lighting Program
	LightingProgram* m_pLightingProgram;

	//Single Directional Light
	DirectionalLight* m_pDirectionalLight;

	//All point lights
	PointLight* m_apPointLights[MAX_POINT_LIGHTS]{};
	unsigned int m_uiCreatedPointLightsCount = 0;

	//All spot lights
	SpotLight* m_apSpotLights[MAX_SPOT_LIGHTS]{};
	unsigned int m_uiCreatedSpotLightCount = 0;

};

#endif // __LIGHTING_MANAGER_H__
