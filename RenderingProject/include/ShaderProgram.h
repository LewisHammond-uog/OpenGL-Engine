#ifndef __SHADER_PROGRAM_H__
#define __SHADER_PROGRAM_H__


//OpenGL includes
#include <glad/glad.h>

//Std includes
#include <vector>

class ShaderProgram {

public:

	//Constructor/Destructor
	ShaderProgram();
	virtual ~ShaderProgram(); //Virtual desctructor can be overriden

	virtual bool Initalise(); //Function to initalise the shader program

protected:

	bool AddShader(GLenum a_shaderType, const char* a_filePath);
	bool CompileProgram(unsigned int a_inputAttributeCount, const char** a_inputAttributes,
						unsigned int a_outputAttributeCount, const char** a_outputAttributes);
	void DeleteCachedShaders();

	//Get the location of a given uniform
	GLint GetUniformLocation(const char* a_UniformName);
	
	//Program ID as known by OpenGL
	GLuint m_glShaderProgramID;

private:

	void LogCompileError(const char* a_errorText) const;

	std::vector<GLuint> m_vLoadedShaders;

};


#endif //!__SHADER_PROGRAM_H__