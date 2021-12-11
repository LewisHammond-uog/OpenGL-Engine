#include "GeometryProgram.h"

bool GeometryProgram::Initialise()
{
	//Base Initalize
	if(!ShaderProgram::Initialise())
	{
		return false;
	}

	//Add Shaders to program
	if(!AddShader(GL_VERTEX_SHADER, "../shaders/geo_vertex.glsl"))
	{
		return false;
	}
	if (!AddShader(GL_GEOMETRY_SHADER, "../shaders/geo_geometry.glsl"))
	{
		return false;
	}
	if (!AddShader(GL_FRAGMENT_SHADER, "../shaders/geo_fragment.glsl"))
	{
		return false;
	}

	//Compile Shaders
	if(!CompileProgram())
	{
		return false;
	}

	//Get uniform locations


}
