#include "LightingManager.h"

//Framework includes
#include "Application_Log.h"

//Project includes
#include <sstream>
#include <string>

#include "BaseLight.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "WorldTransform.h"

//Construct Lighting Manager with Lighting Program
LightingManager::LightingManager(LightingProgram* a_program) :
	m_pLightingProgram(a_program)
{
}

void LightingManager::Update(WorldTransform& a_pWorldTransform)
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
	for(int i = 0; i < m_createdPointLightsCount; ++i)
	{
		m_pointLights[i]->CalculateLocalPosition(a_pWorldTransform);
	}
	//Set lights
	m_pLightingProgram->SetPointLights(m_createdPointLightsCount, m_pointLights);

	//Calculate Spot Light Local Positions and Directions
	for (int i = 0; i < m_createdSpotLightCount; ++i)
	{
		m_spotLights[i]->CalculateLocalDirectionAndPosition(a_pWorldTransform);
	}
	//Set Lights
	m_pLightingProgram->SetSpotLights(m_createdSpotLightCount, m_spotLights);
}

/// <summary>
/// Create a Point Light and add to the Lighting Manager
/// </summary>
/// <param name="a_worldPosition">World Position of the Light</param>
/// <param name="a_colour">Colour of the Light</param>
/// <param name="a_ambientIntensity">Ambient Intensity of the Light</param>
/// <param name="a_diffuseIntensity">Diffuse Intensity of the Light</param>
/// <param name="a_linearAttenuation">Linear Attenuation of the Light</param>
/// <param name="a_exponentialAttenuation">Exponential Attenuation of the Light</param>
/// <returns>The Created Light</returns>
PointLight* LightingManager::CreatePointLight(glm::vec3 a_worldPosition, glm::vec3 a_colour, float a_ambientIntensity,
                                              float a_diffuseIntensity, float a_linearAttenuation, float a_exponentialAttenuation)
{
	if (m_createdPointLightsCount >= MAX_POINT_LIGHTS)
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
	m_pointLights[m_createdPointLightsCount] = new PointLight();
	m_pointLights[m_createdPointLightsCount]->m_worldPosition = a_worldPosition;
	m_pointLights[m_createdPointLightsCount]->m_lightColour = a_colour;
	m_pointLights[m_createdPointLightsCount]->m_ambientIntensity = a_ambientIntensity;
	m_pointLights[m_createdPointLightsCount]->m_diffuseIntensity = a_diffuseIntensity;
	m_pointLights[m_createdPointLightsCount]->m_attenuation.m_linear = a_linearAttenuation;
	m_pointLights[m_createdPointLightsCount]->m_attenuation.m_exponential = a_exponentialAttenuation;

	m_createdPointLightsCount++;
	return m_pointLights[m_createdPointLightsCount - 1];
}

SpotLight* LightingManager::CreateSpotLight(glm::vec3 a_worldPosition, glm::vec3 a_worldDirection, glm::vec3 a_colour,
	float a_diffuseIntensity, float a_linearAttenuation, float a_exponentialAttenuation, float a_cutOffAngle)
{
	if (m_createdSpotLightCount >= MAX_SPOT_LIGHTS)
	{
		//Log warning that we are tyring to create too many lights
		Application_Log* log = Application_Log::Get();
		if (log != nullptr)
		{
			log->addLog(LOG_WARNING, "Trying to create too many spot lights, max capacity is %i", MAX_SPOT_LIGHTS);
		}

		return nullptr;
	}

	//Create light and set properties
	m_spotLights[m_createdSpotLightCount] = new SpotLight();
	m_spotLights[m_createdSpotLightCount]->m_worldPosition = a_worldPosition;
	m_spotLights[m_createdSpotLightCount]->m_worldDirection = a_worldDirection;
	m_spotLights[m_createdSpotLightCount]->m_lightColour = a_colour;
	m_spotLights[m_createdSpotLightCount]->m_diffuseIntensity = a_diffuseIntensity;
	m_spotLights[m_createdSpotLightCount]->m_attenuation.m_linear = a_linearAttenuation;
	m_spotLights[m_createdSpotLightCount]->m_attenuation.m_exponential = a_exponentialAttenuation;
	m_spotLights[m_createdSpotLightCount]->m_cutoff = a_cutOffAngle;

	m_createdSpotLightCount++;
	return m_spotLights[m_createdSpotLightCount - 1];
}

/// <summary>
/// Render the debug ImGui window
///	To show the lighting debug window
/// </summary>
void LightingManager::RenderImguiWindow()
{
	ImGui::Begin("Lighting Settings");
	if (ImGui::CollapsingHeader("Lighting Settings")) {
		if(ImGui::CollapsingHeader("Point Lights"))
		{
			for(int i = 0; i < std::size(m_pointLights); ++i)
			{
				//Do not draw if light is null
				if(m_pointLights[i] == nullptr)
				{
					continue;
				}

				//Construct header name
				std::stringstream headerSS;
				headerSS << "Light #" << i << std::endl;
				
				if (ImGui::CollapsingHeader(headerSS.str().c_str())) {
					DrawImguiPointLightSetting(m_pointLights[i]);
				}
			}
		}

		if (ImGui::CollapsingHeader("Spot Lights"))
		{
			for (int i = 0; i < std::size(m_spotLights); ++i)
			{
				//Do not draw if light is null
				if (m_spotLights[i] == nullptr)
				{
					continue;
				}

				//Construct header name
				std::stringstream headerSS;
				headerSS << "Light #" << i << std::endl;

				if (ImGui::CollapsingHeader(headerSS.str().c_str())) {
					DrawImguiSpotLightSetting(m_spotLights[i]);
				}
			}
		}

	}
	ImGui::End();
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
	ImGui::DragFloat3("Position", reinterpret_cast<float*>(&a_pLight->m_worldPosition), posDragInterval);

	ImGui::Spacing();

	//Intensity
	constexpr float intensityDragInterval = 0.01f;
	ImGui::DragFloat("Ambient Intensity", &a_pLight->m_ambientIntensity, intensityDragInterval);
	ImGui::DragFloat("Diffuse Intensity", &a_pLight->m_diffuseIntensity, intensityDragInterval);

	ImGui::Spacing();

	//Colour
	ImGui::ColorEdit3("Light Colour", reinterpret_cast<float*>(&a_pLight->m_lightColour));

	ImGui::Spacing();

	//Falloff
	constexpr float lightFalloffInterval = 0.001f;
	ImGui::DragFloat("Constant Falloff", &a_pLight->m_attenuation.m_constant, lightFalloffInterval);
	ImGui::DragFloat("Linear Falloff", &a_pLight->m_attenuation.m_linear, lightFalloffInterval);
	ImGui::DragFloat("Exp Falloff", &a_pLight->m_attenuation.m_exponential, lightFalloffInterval);

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
	ImGui::DragFloat3("Direction", reinterpret_cast<float*>(&a_pLight->m_worldDirection), dirDragInterval);

	//Angle
	constexpr float cutoffDragInterval = 0.1f;
	ImGui::DragFloat("Cutoff Angle", &a_pLight->m_cutoff, cutoffDragInterval);

	ImGui::PopID();
}

