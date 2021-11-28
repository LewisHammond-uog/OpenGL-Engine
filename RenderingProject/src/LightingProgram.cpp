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
	//Directional Light Uniform Locations
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

void LightingProgram::SetDirectionalLight(const DirectionalLight* a_pLight)
{
	glUniform3f(dirLightLocation.colour, a_pLight->m_lightColour.r, a_pLight->m_lightColour.g, a_pLight->m_lightColour.b);
	glUniform1f(dirLightLocation.ambientIntensity, a_pLight->m_ambientIntensity);
	glm::vec3 localDirection = a_pLight->GetLocalDirection();
	glUniform3fv(dirLightLocation.direction, 1, glm::value_ptr(localDirection));
	glUniform1f(dirLightLocation.diffuseIntensity, a_pLight->m_diffuseIntensity);
}

/// <summary>
/// Set the point lights in the shader
/// </summary>
/// <param name="a_numLights"></param>
/// <param name="a_pLights">Array of Point Light Pointers</param>
void LightingProgram::SetPointLights(unsigned a_numLights, PointLight* a_pLights[])
{
	//Set number of lights
	glUniform1i(m_numPointLightsLocation, a_numLights);

	//Loop all of the lights and set their properties
	for(unsigned int i = 0; i < a_numLights; i++)
	{
		//Get the light pointer in the array of light pointers
		PointLight* light = a_pLights[i];

		glUniform3f(pointLightsLocation[i].colour, light->m_lightColour.r, light->m_lightColour.g, light->m_lightColour.b); //Colour
		glUniform1f(pointLightsLocation[i].ambientIntensity, light->m_ambientIntensity); //Ambient Intensity
		glUniform1f(pointLightsLocation[i].diffuseIntensity, light->m_diffuseIntensity); //Diffuse Intensity
		const glm::vec3 localPos = light->GetLocalPosition();
		glUniform3fv(pointLightsLocation[i].position, 1, glm::value_ptr(localPos)); //Light Local Position
		glUniform1f(pointLightsLocation[i].attenuation.constant, light->m_attenuation.m_constant); //Constant Attenuation
		glUniform1f(pointLightsLocation[i].attenuation.linear, light->m_attenuation.m_linear); //Linear Attenuation
		glUniform1f(pointLightsLocation[i].attenuation.exponential, light->m_attenuation.m_exponential); //Exponential Attenuation
	}
}

void LightingProgram::SetSpotLights(unsigned a_numLights, SpotLight* a_pLights[]) 
{
	//Set number of lights
	glUniform1i(m_numSpotLightsLocation, a_numLights);

	//Loop all of the lights and set their properties
	for (unsigned int i = 0; i < a_numLights; i++)
	{
		//Get the light pointer in the array of light pointers
		SpotLight* light = a_pLights[i];

		glUniform3fv(spotLightLocations[i].colour, 1, glm::value_ptr(light->m_lightColour));
		glUniform1f(spotLightLocations[i].ambientIntensity, light->m_ambientIntensity);
		glUniform1f(spotLightLocations[i].diffuseIntensity, light->m_diffuseIntensity);
		//Position
		glUniform3fv(spotLightLocations[i].position, 1, glm::value_ptr(light->GetLocalPosition()));
		//Direction
		glm::vec3 localDirection = glm::normalize(light->GetLocalDirection());
		glUniform3fv(spotLightLocations[i].direction, 1, glm::value_ptr(localDirection));
		glUniform1f(spotLightLocations[i].cutoff, cosf(WorldTransform::ToRadians(light->m_cutoff)));
		glUniform1f(spotLightLocations[i].attenuation.constant, light->m_attenuation.m_constant);
		glUniform1f(spotLightLocations[i].attenuation.linear, light->m_attenuation.m_linear);
		glUniform1f(spotLightLocations[i].attenuation.exponential, light->m_attenuation.m_exponential);
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

void LightingProgram::SetMaterial(const Material& a_material) const
{
	glUniform3f(materialLocation.ambientColour, a_material.m_ambientColour.r, a_material.m_ambientColour.g, a_material.m_ambientColour.b);
	glUniform3f(materialLocation.diffuseColour, a_material.m_diffuseColour.r, a_material.m_diffuseColour.g, a_material.m_diffuseColour.b);
	glUniform3f(materialLocation.specularColour, a_material.m_specularColour.r, a_material.m_specularColour.g, a_material.m_specularColour.b);
}


