#ifndef __SHADOW_FBO_H__
#define __SHADOW_FBO_H__

//OpenGL includes
#include <glad/glad.h>

class ShadowFBO
{
public:
	ShadowFBO();
	~ShadowFBO();

	bool Init(unsigned int a_width, unsigned int a_height);

	void BindForWriting();
	void BindForReading();

	GLuint m_depthTexture; //Handle to Shadow Map Texture

private:
	GLuint m_fbo; //Handle to Frame Buffer Object

};

#endif //__SHADOW_FBO_H__
