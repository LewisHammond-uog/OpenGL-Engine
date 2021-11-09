#include "LightingProgram.h"

#include "Material.h"
#include "BaseLight.h"

#include <glm/gtc/type_ptr.hpp>

#include "DirectionalLight.h"

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
	m_samplerLocation = GetUniformLocation("uSampler");
	//Shader Material Uniform Locations
	materialLocation.ambientColour = GetUniformLocation("uMaterial.AmbientColour");
	materialLocation.diffuseColour = GetUniformLocation("uMaterial.DiffuseColour");
	//Shader Light Uniform Locations
	dirLightLocation.colour = GetUniformLocation("uDirectionalLight.Colour");
	dirLightLocation.ambientIntensity = GetUniformLocation("uDirectionalLight.AmbientIntensity");
	dirLightLocation.direction = GetUniformLocation("uDirectionalLight.Direction");
	dirLightLocation.diffuseIntensity = GetUniformLocation("uDirectionalLight.DiffuseIntensity");


	return true;
}

void LightingProgram::SetWorldViewPoint(const glm::mat4 a_worldViewPoint)
{
	glUniformMatrix4fv(m_worldViewPointLocation, 1, GL_FALSE, glm::value_ptr(a_worldViewPoint));
}

void LightingProgram::SetTextureUnit(const unsigned int a_textureUnit)
{
	glUniform1i(m_samplerLocation, a_textureUnit);
}

void LightingProgram::SetDirectionalLight(const DirectionalLight& a_light)
{
	glUniform3f(dirLightLocation.colour, a_light.m_lightColour.r, a_light.m_lightColour.g, a_light.m_lightColour.b);
	glUniform1f(dirLightLocation.ambientIntensity, a_light.m_ambientIntensity);
	glm::vec3 localDirection = a_light.GetLocalDirection();
	glUniform3fv(dirLightLocation.direction, 1, glm::value_ptr(localDirection));
	glUniform1f(dirLightLocation.diffuseIntensity, a_light.m_diffuseIntensity);
}

/*
void LightingProgram::SetLight(const BaseLight& a_light)
{
	glUniform3f(m_lightColourLocation, a_light.m_lightColour.r, a_light.m_lightColour.g, a_light.m_lightColour.b);
	glUniform1f(m_lightAmbitentIntensityLocation, a_light.m_lightIntensity);
}
*/

void LightingProgram::SetMaterial(const Material& a_material)
{
	glUniform3f(materialLocation.ambientColour, a_material.m_ambientColour.r, a_material.m_ambientColour.g, a_material.m_ambientColour.b);
	glUniform3f(materialLocation.diffuseColour, a_material.m_diffuseColour.r, a_material.m_diffuseColour.g, a_material.m_diffuseColour.b);
}
