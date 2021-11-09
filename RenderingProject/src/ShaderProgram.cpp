#include "ShaderProgram.h"

//Framework Includes
#include "Utilities.h"
#include "Application_Log.h"


ShaderProgram::ShaderProgram() 
{
	//Set shader program to be invalid
	m_glShaderProgramID = static_cast<GLuint>(0);
}

ShaderProgram::~ShaderProgram() 
{
	//Delete any cached shaders
	DeleteCachedShaders();

	//Delete the program
	glDeleteProgram(m_glShaderProgramID);
}

bool ShaderProgram::Initialise() 
{
	//Create an Open GL Program
	m_glShaderProgramID = glCreateProgram();
	if(m_glShaderProgramID == 0)
	{
		LogCompileError("Did not create shader program");
		return false;
	}

	return true;

	/* General workflow
	 * - Create Shader Program (done here)
	 * - Add Shaders
	 * - Compile Program
	 * - Delete cahced shaders
	 */
}

void ShaderProgram::UseProgram() const
{
	glUseProgram(m_glShaderProgramID);
}

bool ShaderProgram::CompileProgram() const
{
	GLint success = GL_FALSE;

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
	if(shaderSource == nullptr)
	{
		LogCompileError("Unable to Read Shader Source from File");
		return false;
	}

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
	m_vLoadedShaders.clear();
}

GLint ShaderProgram::GetUniformLocation(const char* a_uniformName) const
{
	return glGetUniformLocation(m_glShaderProgramID, a_uniformName);
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
