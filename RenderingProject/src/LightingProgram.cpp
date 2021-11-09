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
	m_diffuseSamplerLocation = GetUniformLocation("uDiffuseSampler");
	m_specularSamplerLocation = GetUniformLocation("uSpecularSampler");
	//Shader Material Uniform Locations
	materialLocation.ambientColour = GetUniformLocation("uMaterial.AmbientColour");
	materialLocation.diffuseColour = GetUniformLocation("uMaterial.DiffuseColour");
	materialLocation.specularColour = GetUniformLocation("uMaterial.SpecularColour");
	//Shader Light Uniform Locations
	dirLightLocation.colour = GetUniformLocation("uDirectionalLight.Colour");
	dirLightLocation.ambientIntensity = GetUniformLocation("uDirectionalLight.AmbientIntensity");
	dirLightLocation.direction = GetUniformLocation("uDirectionalLight.Direction");
	dirLightLocation.diffuseIntensity = GetUniformLocation("uDirectionalLight.DiffuseIntensity");
	//Camera Uniform Locations
	m_cameraPositionLocation = GetUniformLocation("uCameraLocalPos");

	return true;
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
