#ifndef __SHADER_PROGRAM_H__
#define __SHADER_PROGRAM_H__


//OpenGL includes
#include <glad/glad.h>

//Std includes
#include <vector>

class ShaderProgram {

public:

	virtual bool Initialise(); //Function to Initialise the shader program

	void UseProgram() const; //Set this to be the active OpenGL Program

protected:
	//Constructor/Destructor
	ShaderProgram();
	virtual ~ShaderProgram(); //Virtual desctructor can be overriden

	bool AddShader(GLenum a_shaderType, const char* a_filePath);
	bool CompileProgram() const;
	void DeleteCachedShaders();

	//Get the location of a given uniform
	GLint GetUniformLocation(const char* a_uniformName) const;
	
	//Program ID as known by OpenGL, handle to OpenGL Program
	GLuint m_glShaderProgramID;

private:

	void LogCompileError(const char* a_errorText) const;

	std::vector<GLuint> m_vLoadedShaders;

};


#endif //!__SHADER_PROGRAM_H__