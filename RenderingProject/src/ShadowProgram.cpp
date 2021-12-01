#include "ShadowProgram.h"

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
	m_worldViewPointLocation = GetUniformLocation("uWorldViewPoint"); 
	m_depthSamplerLocation = GetUniformLocation("uDepthSampler");

	return true;
}
