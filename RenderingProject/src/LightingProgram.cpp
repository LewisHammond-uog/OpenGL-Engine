#include "LightingProgram.h"

#include "Material.h"

#include <glm/gtc/type_ptr.hpp>

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
	m_lightColourLocation = GetUniformLocation("uLight.Colour");
	m_lightAmbitentIntensityLocation = GetUniformLocation("uLight.AmbientIntensity");
	m_materialAmbientColourLocation = GetUniformLocation("uMaterial.AmbientColour");

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

void LightingProgram::SetLight(const BaseLight& a_light)
{
	glUniform3f(m_lightColourLocation, a_light.m_lightColour.r, a_light.m_lightColour.g, a_light.m_lightColour.b);
	glUniform1f(m_lightAmbitentIntensityLocation, a_light.m_lightIntensity);
}


void LightingProgram::SetMaterial(const Material& a_material)
{
	glUniform3f(m_materialAmbientColourLocation, a_material.m_ambientColour.r, a_material.m_ambientColour.g, a_material.m_ambientColour.b);
}
