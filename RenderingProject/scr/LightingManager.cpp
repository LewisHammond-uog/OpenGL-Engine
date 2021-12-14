#include "LightingManager.h"

//Framework includes
#include "Application_Log.h"

//Project includes
#include <sstream>
#include <string>

#include "BaseLight.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "Gizmos.h"
#include "WorldTransform.h"

//Construct Lighting Manager with Lighting Program
LightingManager::LightingManager(LightingProgram* a_pProgram) :
	m_pLightingProgram(a_pProgram),
	m_pDirectionalLight(nullptr)
{
}

LightingManager::~LightingManager()
{
	//Delete all of the created lights
	for (PointLight*& pointLight : m_apPointLights)
	{
		delete pointLight;
	}
	for (SpotLight*& spotLight : m_apSpotLights)
	{
		delete spotLight;
	}

	delete m_pDirectionalLight;
}

void LightingManager::Update(WorldTransform& a_rWorldTransform)
{
	if(m_pLightingProgram == nullptr)
	{
		Application_Log* log = Application_Log::Get();
		if (log != nullptr)
		{
			log->addLog(LOG_ERROR, "Invalid Lighting Program, Cannot update lights");
		}
		return;
	}

	//Calculate Point Light Local Positions
	for(int i = 0; i < m_uiCreatedPointLightsCount; ++i)
	{
		m_apPointLights[i]->CalculateLocalPosition(a_rWorldTransform);
	}
	//Set lights
	m_pLightingProgram->SetPointLights(m_uiCreatedPointLightsCount, m_apPointLights);

	//Calculate Spot Light Local Positions and Directions
	for (int i = 0; i < m_uiCreatedSpotLightCount; ++i)
	{
		m_apSpotLights[i]->CalculateLocalDirectionAndPosition(a_rWorldTransform);
	}
	//Set Lights
	m_pLightingProgram->SetSpotLights(m_uiCreatedSpotLightCount, m_apSpotLights);

	//Set Directional Light
	if(m_pDirectionalLight)
	{
		m_pDirectionalLight->CalculateLocalDirection(a_rWorldTransform);
		m_pLightingProgram->SetDirectionalLight(m_pDirectionalLight);
	}
}

/// <summary>
/// Create the single directonal light in the scene
/// </summary>
/// <returns></returns>
DirectionalLight* LightingManager::CreateDirectionalLight(glm::vec3 a_v3WorldDirection, glm::vec3 a_v3Colour, float a_fAmbientIntensity, float a_fDiffuseIntensity)
{
	//If a directonal light has not been allocated create one
	if(m_pDirectionalLight == nullptr)
	{
		m_pDirectionalLight = new DirectionalLight();
		m_pDirectionalLight->m_v3WorldDirection = a_v3WorldDirection;
		m_pDirectionalLight->m_v3LightColour = a_v3Colour;
		m_pDirectionalLight->m_fAmbientIntensity = a_fAmbientIntensity;
		m_pDirectionalLight->m_fDiffuseIntensity = a_fDiffuseIntensity;
	}

	return m_pDirectionalLight;
}

/// <summary>
/// Create a Point Light and add to the Lighting Manager
/// </summary>
/// <param name="a_v3WorldPosition">World Position of the Light</param>
/// <param name="a_v3Colour">Colour of the Light</param>
/// <param name="a_fAmbientIntensity">Ambient Intensity of the Light</param>
/// <param name="a_fDiffuseIntensity">Diffuse Intensity of the Light</param>
/// <param name="a_fLinearAttenuation">Linear Attenuation of the Light</param>
/// <param name="a_fExponentialAttenuation">Exponential Attenuation of the Light</param>
/// <returns>The Created Light</returns>
PointLight* LightingManager::CreatePointLight(glm::vec3 a_v3WorldPosition, glm::vec3 a_v3Colour, float a_fAmbientIntensity,
                                              float a_fDiffuseIntensity, float a_fLinearAttenuation, float a_fExponentialAttenuation)
{
	if (m_uiCreatedPointLightsCount >= MAX_POINT_LIGHTS)
	{
		//Log warning that we are tyring to create too many lights
		Application_Log* log = Application_Log::Get();
		if(log != nullptr)
		{
			log->addLog(LOG_WARNING, "Trying to create too many point lights, max capacity is %i", MAX_POINT_LIGHTS);
		}

		return nullptr;
	}

	//Create light and set properties
	m_apPointLights[m_uiCreatedPointLightsCount] = new PointLight();
	m_apPointLights[m_uiCreatedPointLightsCount]->m_v3WorldPosition = a_v3WorldPosition;
	m_apPointLights[m_uiCreatedPointLightsCount]->m_v3LightColour = a_v3Colour;
	m_apPointLights[m_uiCreatedPointLightsCount]->m_fAmbientIntensity = a_fAmbientIntensity;
	m_apPointLights[m_uiCreatedPointLightsCount]->m_fDiffuseIntensity = a_fDiffuseIntensity;
	m_apPointLights[m_uiCreatedPointLightsCount]->m_attenuation.m_fLinear = a_fLinearAttenuation;
	m_apPointLights[m_uiCreatedPointLightsCount]->m_attenuation.m_fExponential = a_fExponentialAttenuation;

	m_uiCreatedPointLightsCount++;
	return m_apPointLights[m_uiCreatedPointLightsCount - 1];
}

SpotLight* LightingManager::CreateSpotLight(glm::vec3 a_v3WorldPosition, glm::vec3 a_v3WorldDirection, glm::vec3 a_v3Colour,
	float a_fAmbientIntensity, float a_fDiffuseIntensity, float a_fLinearAttenuation, float a_fExponentialAttenuation, float a_fCutOffAngle)
{
	if (m_uiCreatedSpotLightCount >= MAX_SPOT_LIGHTS)
	{
		//Log warning that we are tyring to create too many lights
		Application_Log* log = Application_Log::Get();
		if (log != nullptr)
		{
			log->addLog(LOG_WARNING, "Trying to create too many spot lights, max capacity is %i.\nLight will not be created.", MAX_SPOT_LIGHTS);
		}

		return nullptr;
	}

	//Create light and set properties
	m_apSpotLights[m_uiCreatedSpotLightCount] = new SpotLight();
	m_apSpotLights[m_uiCreatedSpotLightCount]->m_v3WorldPosition = a_v3WorldPosition;
	m_apSpotLights[m_uiCreatedSpotLightCount]->m_v3WorldDirection = a_v3WorldDirection;
	m_apSpotLights[m_uiCreatedSpotLightCount]->m_v3LightColour = a_v3Colour;
	m_apSpotLights[m_uiCreatedSpotLightCount]->m_fAmbientIntensity = a_fAmbientIntensity;
	m_apSpotLights[m_uiCreatedSpotLightCount]->m_fDiffuseIntensity = a_fDiffuseIntensity;
	m_apSpotLights[m_uiCreatedSpotLightCount]->m_attenuation.m_fLinear = a_fLinearAttenuation;
	m_apSpotLights[m_uiCreatedSpotLightCount]->m_attenuation.m_fExponential = a_fExponentialAttenuation;
	m_apSpotLights[m_uiCreatedSpotLightCount]->m_fCutoff = a_fCutOffAngle;

	m_uiCreatedSpotLightCount++;
	return m_apSpotLights[m_uiCreatedSpotLightCount - 1];
}

/// <summary>
/// Render the debug ImGui window
///	To show the lighting debug window
/// </summary>
void LightingManager::RenderImguiWindow()
{
	ImGui::Begin("Lighting Settings");
	if (ImGui::CollapsingHeader("Lighting Settings")) {

		ImGui::Checkbox("Draw Light Gizmos", &m_bDrawLightGizmos);
		ImGui::Spacing();

		//Draw Directional Light
		if (ImGui::CollapsingHeader("Directional Light"))
		{
			if (m_pDirectionalLight)
			{
				DrawImguiDirectionalLightSetting(m_pDirectionalLight);
			}
		}

		//Draw Point Lights
		if(ImGui::CollapsingHeader("Point Lights"))
		{
			for(int i = 0; i < std::size(m_apPointLights); ++i)
			{
				//Do not draw if light is null
				if(m_apPointLights[i] == nullptr)
				{
					continue;
				}

				//Construct header name
				std::stringstream headerSS;
				headerSS << "Light #" << i << std::endl;
				
				if (ImGui::CollapsingHeader(headerSS.str().c_str())) {
					DrawImguiPointLightSetting(m_apPointLights[i]);
				}
			}
		}

		//Draw Spot Lights
		if (ImGui::CollapsingHeader("Spot Lights"))
		{
			for (int i = 0; i < std::size(m_apSpotLights); ++i)
			{
				//Do not draw if light is null
				if (m_apSpotLights[i] == nullptr)
				{
					continue;
				}

				//Construct header name
				std::stringstream headerSS;
				headerSS << "Light #" << i << std::endl;

				if (ImGui::CollapsingHeader(headerSS.str().c_str())) {
					DrawImguiSpotLightSetting(m_apSpotLights[i]);
				}
			}
		}

	}
	ImGui::End();
}

void LightingManager::DrawLightingGizmos()
{

	//Early out if we shouldn't draw gizmos
	if(!m_bDrawLightGizmos)
	{
		return;
	}

	constexpr float fGizmosRadius = 0.5f;
	constexpr int iPointLightSpherePrecision = 25;

	//Loop Point Lights
	for (PointLight* pPointLight : m_apPointLights)
	{
		if(pPointLight == nullptr)
		{
			continue;
		}

		Gizmos::addSphere(pPointLight->m_v3WorldPosition, iPointLightSpherePrecision, iPointLightSpherePrecision,fGizmosRadius, glm::vec4(pPointLight->m_v3LightColour, 1));
	}

	//Loop Spot Lights
	for (SpotLight* pSpotLight : m_apSpotLights)
	{
		if(pSpotLight == nullptr)
		{
			continue;;
		}

		Gizmos::addBox(pSpotLight->m_v3WorldPosition, glm::vec3(fGizmosRadius), true, glm::vec4(pSpotLight->m_v3LightColour, 1));
	}

}

/// <summary>
/// Draw the settings of a directional light in an Imgui Context
/// </summary>
/// <param name="a_pLight"></param>
void LightingManager::DrawImguiDirectionalLightSetting(DirectionalLight* a_pLight) const
{
	ImGui::PushID(a_pLight); //Push a unqiqe ID to imgui to stop the ID stack from generating the same ID for all lights
	ImGui::Spacing();

	//Direction
	constexpr float dirDragInterval = 0.01f;
	ImGui::DragFloat3("Direction", reinterpret_cast<float*>(&a_pLight->m_v3WorldDirection), dirDragInterval);
	ImGui::Spacing();

	//Intensity
	constexpr float intensityDragInterval = 0.01f;
	ImGui::DragFloat("Ambient Intensity", &a_pLight->m_fAmbientIntensity, intensityDragInterval);
	ImGui::DragFloat("Diffuse Intensity", &a_pLight->m_fDiffuseIntensity, intensityDragInterval);

	//Colour
	ImGui::ColorEdit3("Light Colour", reinterpret_cast<float*>(&a_pLight->m_v3LightColour));
	ImGui::Spacing();

	ImGui::PopID();
}

/// <summary>
/// Draw the settings for a point light in an Imgui Context
/// </summary>
void LightingManager::DrawImguiPointLightSetting(PointLight* a_pLight) const
{
	ImGui::PushID(a_pLight); //Push a unqiqe ID to imgui to stop the ID stack from generating the same ID for all lights
	ImGui::Spacing();

	//Position
	constexpr float posDragInterval = 0.1f; //Interval for the dragging the position sliders
	ImGui::DragFloat3("Position", reinterpret_cast<float*>(&a_pLight->m_v3WorldPosition), posDragInterval);

	ImGui::Spacing();

	//Intensity
	constexpr float intensityDragInterval = 0.01f;
	ImGui::DragFloat("Ambient Intensity", &a_pLight->m_fAmbientIntensity, intensityDragInterval);
	ImGui::DragFloat("Diffuse Intensity", &a_pLight->m_fDiffuseIntensity, intensityDragInterval);

	ImGui::Spacing();

	//Colour
	ImGui::ColorEdit3("Light Colour", reinterpret_cast<float*>(&a_pLight->m_v3LightColour));

	ImGui::Spacing();

	//Falloff
	constexpr float lightFalloffInterval = 0.001f;
	ImGui::DragFloat("Constant Falloff", &a_pLight->m_attenuation.m_fConstant, lightFalloffInterval);
	ImGui::DragFloat("Linear Falloff", &a_pLight->m_attenuation.m_fLinear, lightFalloffInterval);
	ImGui::DragFloat("Exp Falloff", &a_pLight->m_attenuation.m_fExponential, lightFalloffInterval);

	ImGui::Spacing();
	ImGui::PopID();
}

/// <summary>
/// Draw the settings for a spot light in an Imgui Context
/// </summary>
/// <param name="a_pLight"></param>
void LightingManager::DrawImguiSpotLightSetting(SpotLight* a_pLight) const
{
	ImGui::PushID(a_pLight);
	//Draw settings for point light - because spot light inherits from point light
	DrawImguiPointLightSetting(a_pLight);

	ImGui::Spacing();

	//Direction
	constexpr float dirDragInterval = 0.01f;
	ImGui::DragFloat3("Direction", reinterpret_cast<float*>(&a_pLight->m_v3WorldDirection), dirDragInterval);

	//Angle
	constexpr float cutoffDragInterval = 0.1f;
	ImGui::DragFloat("Cutoff Angle", &a_pLight->m_fCutoff, cutoffDragInterval);

	ImGui::PopID();
}

