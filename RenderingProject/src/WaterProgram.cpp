#include "WaterProgram.h"

//Imgui Includes
#include "imgui.h"

//GLM Includes
#include <glm/gtc/type_ptr.hpp>

bool WaterProgram::Initialise()
{
	//Initalise Shader Program
	if (!ShaderProgram::Initialise())
	{
		return false;
	}

	//Add shaders to program
	if (!AddShader(GL_VERTEX_SHADER, "./shaders/water_vertex.glsl"))
	{
		return false;
	}
	if (!AddShader(GL_GEOMETRY_SHADER, "./shaders/water_geometry.glsl"))
	{
		return false;
	}
	if (!AddShader(GL_FRAGMENT_SHADER, "./shaders/water_fragment.glsl"))
	{
		return false;
	}


	//Compile Program
	if (!CompileProgram())
	{
		return false;
	}

	//Get Uniform Locations
	m_worldViewPointLocation = GetUniformLocation("uWorldViewPoint");
	m_worldCamPosLocation = GetUniformLocation("cameraPosition");
	m_timeLocation = GetUniformLocation("uTime");
	m_lightDirectionLocation = GetUniformLocation("ulightDirection");
	m_lightColourLocation = GetUniformLocation("ulightColour");

	return true;
}


/// <summary>
/// Set the world view point matrix
/// </summary>
/// <param name="a_worldViewPoint"></param>
void WaterProgram::SetWorldViewPoint(const glm::mat4 a_worldViewPoint) const
{
	glUniformMatrix4fv(m_worldViewPointLocation, 1, GL_FALSE, glm::value_ptr(a_worldViewPoint));
}


/// <summary>
/// Set the world camera position
/// </summary>
/// <param name="a_camPos"></param>
void WaterProgram::SetWorldCameraPos(const glm::vec3 a_camPos) const
{
	glUniform3fv(m_worldCamPosLocation, 1, glm::value_ptr(a_camPos));
}

/// <summary>
/// Set the time for the water shader
/// </summary>
/// <param name="a_time"></param>
void WaterProgram::SetTime(const float a_time)
{
	glUniform1f(m_timeLocation, a_time);
}

/// <summary>
/// Set the direction for the light for the water
/// </summary>
/// <param name="a_lightDirection"></param>
void WaterProgram::SetLightDirection(const glm::vec3 a_lightDirection) const
{
	glUniform3fv(m_lightDirectionLocation, 1, glm::value_ptr(a_lightDirection));
}

/// <summary>
/// Set the colour of the light
/// </summary>
/// <param name="a_lightColour"></param>
void WaterProgram::SetLightColour(const glm::vec3 a_lightColour) const
{
	glUniform3fv(m_lightColourLocation, 1, glm::value_ptr(a_lightColour));
}
