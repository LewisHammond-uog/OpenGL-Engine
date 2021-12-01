#include "ShadowProgram.h"

#include <glm/gtc/type_ptr.hpp>

bool ShadowProgram::Initialise()
{
	//Call base initalisation
	if(!ShaderProgram::Initialise())
	{
		return false;
	}

	//Add Shaders to program
	if(!AddShader(GL_VERTEX_SHADER, "../shaders/shadow_vertex.glsl"))
	{
		return false;
	}
	//if(!AddShader(GL_FRAGMENT_SHADER, "../shaders/shadow_fragment.glsl"))
	//{
	//	return false;
	//}

	//Compile Shader
	if(!CompileProgram())
	{
		return false;
	}

	//Get Uniform Locations
	m_lightViewPointLocation = GetUniformLocation("uLightViewPoint"); 
	m_depthSamplerLocation = GetUniformLocation("uDepthSampler");

	return true;
}

void ShadowProgram::SetLightViewPoint(glm::mat4 a_matrix)
{
	glUniformMatrix4fv(m_lightViewPointLocation, 1, GL_FALSE, glm::value_ptr(a_matrix));
}
