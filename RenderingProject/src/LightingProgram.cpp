#include "LightingProgram.h"
#include "Macros.h"

//GLM Includes
#include <glm/gtc/type_ptr.hpp>

//Project Includes
#include "Material.h"
#include "BaseLight.h"
#include "DirectionalLight.h"
#include "WorldTransform.h"
#include "PointLight.h"
#include "SpotLight.h"


bool LightingProgram::Initialise()
{
	//Initalise Shader Program
	if(!ShaderProgram::Initialise())
	{
		return false;
	}

	//Add shaders to program
	if (!AddShader(GL_VERTEX_SHADER, "../shaders/lighting_vertex.glsl"))
	{
		return false;
	}

	if(!AddShader(GL_FRAGMENT_SHADER, "../shaders/lighting_fragment.glsl"))
	{
		return false;
	}

	//Compile Program
	if(!CompileProgram())
	{
		return false;
	}

	//Get Uniform Locations
	m_worldViewPointLocation = GetUniformLocation("uWorldViewPoint");
	m_diffuseSamplerLocation = GetUniformLocation("uDiffuseSampler");
	m_specularSamplerLocation = GetUniformLocation("uSpecularSampler");
	//Shader Material Uniform Locations
	materialLocation.ambientColour = GetUniformLocation("uMaterial.AmbientColour");
	materialLocation.diffuseColour = GetUniformLocation("uMaterial.DiffuseColour");
	materialLocation.specularColour = GetUniformLocation("uMaterial.SpecularColour");
	//Shader Light Uniform Locations
	dirLightLocation.colour = GetUniformLocation("uDirectionalLight.base.Colour");
	dirLightLocation.ambientIntensity = GetUniformLocation("uDirectionalLight.base.AmbientIntensity");
	dirLightLocation.direction = GetUniformLocation("uDirectionalLight.Direction");
	dirLightLocation.diffuseIntensity = GetUniformLocation("uDirectionalLight.base.DiffuseIntensity");
	//Point Light Count
	m_numPointLightsLocation = GetUniformLocation("uPointLightCount");
	//Spot Light Count
	m_numSpotLightsLocation = GetUniformLocation("uSpotLightCount");
	//Camera Uniform Locations
	m_cameraPositionLocation = GetUniformLocation("uCameraLocalPos");

	InitalizePointLightUniformLocations();
	InitalizeSpotLightUniformLocations();

	return true;
}

/// <summary>
/// Initalize the locations of point light uniforms
/// </summary>
void LightingProgram::InitalizePointLightUniformLocations()
{
	//Loop all of the point lights
	//Generate the name of the uniform using the index of the current point light
	for(int i =0; i < ARRAY_SIZE_IN_ELEMENTS(pointLightsLocation); i++)
	{
		char name[128] = {};

		//Position
		_snprintf(name, sizeof(name), "uPointLights[%d].localPos", i);
		pointLightsLocation[i].position = GetUniformLocation(name);

		//Colour
		_snprintf(name, sizeof(name), "uPointLights[%d].base.Colour", i);
		pointLightsLocation[i].colour = GetUniformLocation(name);

		//Ambient Intenstity
		_snprintf(name, sizeof(name), "uPointLights[%d].base.AmbientIntensity", i);
		pointLightsLocation[i].ambientIntensity = GetUniformLocation(name);

		//Diffuse Intensity
		_snprintf(name, sizeof(name), "uPointLights[%d].base.DiffuseIntensity", i);
		pointLightsLocation[i].diffuseIntensity = GetUniformLocation(name);

		//Constant attenuation
		_snprintf(name, sizeof(name), "uPointLights[%d].atten.constant", i);
		pointLightsLocation[i].attenuation.constant = GetUniformLocation(name);

		//Linear attenuation
		_snprintf(name, sizeof(name), "uPointLights[%d].atten.linear", i);
		pointLightsLocation[i].attenuation.linear = GetUniformLocation(name);

		//Exponential attenuation
		_snprintf(name, sizeof(name), "uPointLights[%d].atten.exponential", i);
		pointLightsLocation[i].attenuation.exponential = GetUniformLocation(name);
	}
}

/// <summary>
/// Initalize the locactions of the spot light uniforms
/// </summary>
void LightingProgram::InitalizeSpotLightUniformLocations()
{
	//Loop all of the spot lights
	//Generate the name of the uniform using the index of the current spot light
	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(spotLightLocations); i++)
	{
		char name[128] = {};

		//Position
		_snprintf(name, sizeof(name), "uSpotLights[%d].base.localPos", i);
		spotLightLocations[i].position = GetUniformLocation(name);

		//Direction
		_snprintf(name, sizeof(name), "uSpotLights[%d].direction", i);
		spotLightLocations[i].direction = GetUniformLocation(name);

		//Cutoff
		_snprintf(name, sizeof(name), "uSpotLights[%d].cutoff", i);
		spotLightLocations[i].cutoff = GetUniformLocation(name);

		//Colour
		_snprintf(name, sizeof(name), "uSpotLights[%d].base.base.Colour", i);
		spotLightLocations[i].colour = GetUniformLocation(name);

		//Ambient Intensity
		_snprintf(name, sizeof(name), "uSpotLights[%d].base.base.AmbientIntensity", i);
		spotLightLocations[i].ambientIntensity = GetUniformLocation(name);

		//Diffuse Intensity
		_snprintf(name, sizeof(name), "uSpotLights[%d].base.base.DiffuseIntensity", i);
		spotLightLocations[i].diffuseIntensity = GetUniformLocation(name);

		//Constant attenuation
		_snprintf(name, sizeof(name), "uSpotLights[%d].base.atten.constant", i);
		spotLightLocations[i].attenuation.constant = GetUniformLocation(name);
		
		//Linear attenuation
		_snprintf(name, sizeof(name), "uSpotLights[%d].base.atten.linear", i);
		spotLightLocations[i].attenuation.linear = GetUniformLocation(name);

		//Exponential attenuation
		_snprintf(name, sizeof(name), "uSpotLights[%d].base.atten.exponential", i);
		spotLightLocations[i].attenuation.exponential = GetUniformLocation(name);
	}
}

void LightingProgram::SetWorldViewPoint(const glm::mat4 a_worldViewPoint) const
{
	glUniformMatrix4fv(m_worldViewPointLocation, 1, GL_FALSE, glm::value_ptr(a_worldViewPoint));
}

/// <summary>
/// Set the texture unit to use for diffuse colour
/// </summary>
/// <param name="a_textureUnit"></param>
void LightingProgram::SetDiffuseTextureUnit(const unsigned int a_textureUnit) const
{
	glUniform1i(m_diffuseSamplerLocation, a_textureUnit);
}

/// <summary>
/// Set the texture unit to use for the specular power
/// </summary>
/// <param name="a_textureUnit"></param>
void LightingProgram::SetSpecularPowerTextureUnit(const unsigned int a_textureUnit) const
{
	glUniform1i(m_specularSamplerLocation, a_textureUnit);
}

void LightingProgram::SetDirectionalLight(const DirectionalLight& a_light)
{
	glUniform3f(dirLightLocation.colour, a_light.m_lightColour.r, a_light.m_lightColour.g, a_light.m_lightColour.b);
	glUniform1f(dirLightLocation.ambientIntensity, a_light.m_ambientIntensity);
	glm::vec3 localDirection = a_light.GetLocalDirection();
	glUniform3fv(dirLightLocation.direction, 1, glm::value_ptr(localDirection));
	glUniform1f(dirLightLocation.diffuseIntensity, a_light.m_diffuseIntensity);
}

void LightingProgram::SetPointLights(unsigned a_numLights, const PointLight* a_pLights)
{
	//Set number of lights
	glUniform1i(m_numPointLightsLocation, a_numLights);

	//Loop all of the lights and set their properties
	for(unsigned int i = 0; i < a_numLights; i++)
	{
		glUniform3f(pointLightsLocation[i].colour, a_pLights->m_lightColour.r, a_pLights->m_lightColour.g, a_pLights->m_lightColour.b); //Colour
		glUniform1f(pointLightsLocation[i].ambientIntensity, a_pLights->m_ambientIntensity); //Ambient Intensity
		glUniform1f(pointLightsLocation[i].diffuseIntensity, a_pLights->m_diffuseIntensity); //Diffuse Intensity
		//const glm::vec3 localPos = a_pLights[i].GetLocalPosition();
		const glm::vec3 localPos = a_pLights[i].m_worldPosition;
		glUniform3fv(pointLightsLocation[i].position, 1, glm::value_ptr(localPos)); //Light Local Position
		glUniform1f(pointLightsLocation[i].attenuation.constant, a_pLights->m_attenuation.m_constant); //Constant Attenuation
		glUniform1f(pointLightsLocation[i].attenuation.linear, a_pLights->m_attenuation.m_linear); //Linear Attenuation
		glUniform1f(pointLightsLocation[i].attenuation.exponential, a_pLights->m_attenuation.m_exponential); //Exponential Attenuation
	}
}

void LightingProgram::SetSpotLights(unsigned a_numLights, const SpotLight* a_pLights) 
{
	//Set number of lights
	glUniform1i(m_numSpotLightsLocation, a_numLights);

	//Loop all of the lights and set their properties
	for (unsigned int i = 0; i < a_numLights; i++)
	{
		glUniform3fv(spotLightLocations[i].colour, 1, glm::value_ptr(a_pLights[i].m_lightColour));
		glUniform1f(spotLightLocations[i].ambientIntensity, a_pLights[i].m_ambientIntensity);
		glUniform1f(spotLightLocations[i].diffuseIntensity, a_pLights[i].m_diffuseIntensity);
		//Position
		const glm::vec3& localPos = a_pLights->GetLocalPosition();
		glUniform3fv(spotLightLocations[i].position, 1, glm::value_ptr(a_pLights->GetLocalPosition()));
		//Direction
		glm::vec3 localDirection = glm::normalize(a_pLights[i].GetLocalDirection());
		glUniform3fv(spotLightLocations[i].direction, 1, glm::value_ptr(localDirection));
		glUniform1f(spotLightLocations[i].cutoff, cosf(WorldTransform::ToRadians(a_pLights->m_cutoff)));
		glUniform1f(spotLightLocations[i].attenuation.constant, a_pLights[i].m_attenuation.m_constant);
		glUniform1f(spotLightLocations[i].attenuation.linear, a_pLights[i].m_attenuation.m_linear);
		glUniform1f(spotLightLocations[i].attenuation.exponential, a_pLights[i].m_attenuation.m_exponential);
	}
}

/// <summary>
/// Set the local position of the camera in the lighting shader
/// </summary>
/// <param name="a_cameraLocalPosition"></param>
void LightingProgram::SetCameraLocalPos(const glm::vec3& a_cameraLocalPosition) const
{
	glUniform3fv(m_cameraPositionLocation, 1, glm::value_ptr(a_cameraLocalPosition));
}

/*
void LightingProgram::SetLight(const BaseLight& a_light)
{
	glUniform3f(m_lightColourLocation, a_light.m_lightColour.r, a_light.m_lightColour.g, a_light.m_lightColour.b);
	glUniform1f(m_lightAmbitentIntensityLocation, a_light.m_lightIntensity);
}
*/

void LightingProgram::SetMaterial(const Material& a_material) const
{
	glUniform3f(materialLocation.ambientColour, a_material.m_ambientColour.r, a_material.m_ambientColour.g, a_material.m_ambientColour.b);
	glUniform3f(materialLocation.diffuseColour, a_material.m_diffuseColour.r, a_material.m_diffuseColour.g, a_material.m_diffuseColour.b);
	glUniform3f(materialLocation.specularColour, a_material.m_specularColour.r, a_material.m_specularColour.g, a_material.m_specularColour.b);
}


