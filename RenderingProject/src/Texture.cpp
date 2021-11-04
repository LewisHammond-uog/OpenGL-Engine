#include <Texture.h>

//STB Includes
#include <stb_image.h>

//Framework Includes
#include "Application_Log.h"

Texture::Texture(const GLenum a_textureTarget, const std::string& a_fileName) :
	m_fileName(a_fileName),
	m_textureType(a_textureTarget)
{
}

/// <summary>
/// Load the texture from disk
/// </summary>
/// <returns></returns>
bool Texture::Load()
{
	//Set STB to use flip images because OpenGL reads bottom to top
	stbi_set_flip_vertically_on_load(true);
	//Load image from file
	int width = 0, height = 0, channels = 0;
	unsigned char* data = stbi_load(m_fileName.c_str(), &width, &height, &channels, 0);

	if(!data)
	{
		Application_Log* log = Application_Log::Get();
		if(log)
		{
			log->addLog(LOG_ERROR, "Cannot load texture from file %s: %s", m_fileName.c_str(), stbi_failure_reason());
			return false;
		}
	}

	//Generate OpenGL Textures
	glGenTextures(1, &m_textureHandle);
	//Bind texture for active modification
	glBindTexture(m_textureType, m_textureHandle);

	//Determine format to use based on number of channels in the image
	if (m_textureType == GL_TEXTURE_2D) {
		GLint imageFormat = GL_RGBA;
		switch (channels)
		{
		case 1:
			imageFormat = GL_RED;
			break;
		case 3:
			imageFormat = GL_RGB;
			break;
		case 4:
			imageFormat = GL_RGBA;
			break;
		default:
			Application_Log* log = Application_Log::Get();
			if (log)
			{
				log->addLog(LOG_WARNING, "Texture does not have a recognized format, reporting %i channels which is unsupported, defaulting to %i channels", channels, imageFormat);
			}
		}

		//Fill Texture with data
		glTexImage2D(m_textureType, 0, imageFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data);

	}else
	{
		Application_Log* log = Application_Log::Get();
		if (log)
		{
			log->addLog(LOG_ERROR, "Tried to load a unsupported image type in to OpenGL (is not GL_TEXTURE_2D)");
		}
		return false;
	}

	//Set linear mipmapping and wrap repeat
	glTexParameterf(m_textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(m_textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(m_textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(m_textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Unbind texture
	glBindTexture(m_textureType, 0);

	//Free STB Data
	stbi_image_free(data);
	
}

/// <summary>
/// Bind this texture as the active texture
/// </summary>
/// <param name="a_textureUnit"></param>
void Texture::Bind(const GLenum a_textureUnit) const
{
	glActiveTexture(a_textureUnit);
	glBindTexture(m_textureType, m_textureHandle);
}

