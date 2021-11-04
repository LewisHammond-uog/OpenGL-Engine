#include "ShaderProgram.h"

//Framework Includes
#include "Utilities.h"
#include "Application_Log.h"


ShaderProgram::ShaderProgram() 
{
	//Set shader program to be invalid
	m_glShaderProgramID = -1;


}

ShaderProgram::~ShaderProgram() 
{
	//Delete any cached shaders
	DeleteCachedShaders();

	//Delete the program
	glDeleteProgram(m_glShaderProgramID);
}

bool ShaderProgram::Initalise() 
{
	//Create an Open GL Program
	m_glShaderProgramID = glCreateProgram();
	if(m_glShaderProgramID == 0)
	{
		LogCompileError("Did not create shader program");
		return false;
	}

	/* General workflow
	 * - Create Shader Program (done here)
	 * - Add Shaders
	 * - Compile Program
	 * - Delete cahced shaders
	 */
}

bool ShaderProgram::CompileProgram(unsigned int a_inputAttributeCount, const char** a_inputAttributes,
	unsigned int a_outputAttributeCount, const char** a_outputAttributes) {
	
	GLint success = GL_FALSE;

	// specify vertex input attributes
	for (unsigned int i = 0; i < a_inputAttributeCount; ++i)
		glBindAttribLocation(m_glShaderProgramID, i, a_inputAttributes[i]);

	// specify fragment shader outputs
	for (unsigned int i = 0; i < a_outputAttributeCount; ++i)
		glBindFragDataLocation(m_glShaderProgramID, i, a_outputAttributes[i]);

	//Link program together
	glLinkProgram(m_glShaderProgramID);

	//Log any errors
	glGetProgramiv(m_glShaderProgramID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		LogCompileError("Error Linking Shaders");
		return false;
	}

	//Validate Program
	glValidateProgram(m_glShaderProgramID);
	glGetProgramiv(m_glShaderProgramID, GL_VALIDATE_STATUS, &success);
	if (success == GL_FALSE)
	{
		LogCompileError("Invalid Shader Program");
		return false;
	}

	return true;
}

bool ShaderProgram::AddShader(const GLenum a_shaderType, const char* a_filePath) 
{
	GLint success = GL_FALSE;

	//If the program is not valid then abort
	if (m_glShaderProgramID == -1) {
		return false;
	}

	//Load shader from file and create a shader
	unsigned char* shaderSource = Utility::fileToBuffer(a_filePath);
	unsigned int shaderHandle = glCreateShader(a_shaderType);

	//Compile Shader
	glShaderSource(shaderHandle, 1, (const char**)&shaderSource, 0);
	glCompileShader(shaderHandle);

	//Delete shader source 
	delete[] shaderSource;

	//Log failure
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetShaderInfoLog(shaderHandle, infoLogLength, 0, infoLog);

		Application_Log* log = Application_Log::Create();
		if (log != nullptr)
		{

			log->addLog(LOG_LEVEL::LOG_ERROR, "Error: Failed to compile shader!\n : %s", infoLog);
		}

		delete[] infoLog;
		return false;
	}

	//Attach to our program
	glAttachShader(m_glShaderProgramID, shaderHandle);

	//Add to the cached shader list
	m_vLoadedShaders.push_back(shaderHandle);

	return true;
}

void ShaderProgram::DeleteCachedShaders() 
{
	//Delete all of the loaded shaders
	for (const unsigned int& vLoadedShader : m_vLoadedShaders)
	{
		glDeleteShader(vLoadedShader);
	}

	//Empty the cached shader list - they are all deleted now
	m_vLoadedShaders.empty();
}

void ShaderProgram::LogCompileError(const char* a_errorText) const
{
	int infoLogLength = 0;
	glGetProgramiv(m_glShaderProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
	char* infoLog = new char[infoLogLength];

	glGetProgramInfoLog(m_glShaderProgramID, infoLogLength, 0, infoLog);
	Application_Log* log = Application_Log::Create();
	if (log != nullptr)
	{

		log->addLog(LOG_LEVEL::LOG_ERROR, "%s\n : %s", a_errorText, infoLog);
	}

	delete[] infoLog;
}
