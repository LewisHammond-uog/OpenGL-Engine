#include "ShadowFBO.h"

//Project Includes
#include "Macros.h"
#include "Application_Log.h"

//Construct Frame Buffer 
ShadowFBO::ShadowFBO() :
	m_fbo(0),
	m_depthTexture(0)
{
}

ShadowFBO::~ShadowFBO()
{
	//Delete Frame Buffer and Texture
	glDeleteFramebuffers(1, &m_fbo);
	glDeleteTextures(1, &m_depthTexture);
}

/// <summary>
/// Initalize the Shadow Buffer Object
/// </summary>
/// <param name="a_width">Widrt</param>
/// <param name="a_height"></param>
/// <returns></returns>
bool ShadowFBO::Init(unsigned a_width, unsigned a_height)
{
	//Create FBO object and bind it as the active Frame Buffer
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	//Create a depth texture and set it as the active texture
	glGenTextures(1, &m_depthTexture);
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);

	//Set its size, mipmap node and clamp
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, a_width, a_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Bind Frame Buffer to the depth texture
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTexture, 0);

	//Disable writes to the color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	//Check for successful setup
	const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE)
	{
		Application_Log* log = Application_Log::Get();
		if (log != nullptr)
		{
			log->addLog(LOG_LEVEL::LOG_ERROR, "Framebuffer Creation Error! Status: %s", status);
		}
		return false;
	}

	//Unbund FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}


/// <summary>
/// Bind this Frame Buffer for writing
/// </summary>
void ShadowFBO::BindForWriting()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

/// <summary>
/// Bind this Frame Buffer for reading
/// </summary>
void ShadowFBO::BindForReading()
{
	glActiveTexture(SHADOW_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
}
