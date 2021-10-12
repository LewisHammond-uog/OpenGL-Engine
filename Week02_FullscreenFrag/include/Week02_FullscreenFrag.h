#ifndef __Week02_FullscreenFrag_H_
#define __Week02_FullscreenFrag_H_

#include "Application.h"
#include <glm/glm.hpp>

// Derived application class that wraps up all globals neatly
class Week02_FullscreenFrag : public Application
{
public:

	Week02_FullscreenFrag();
	virtual ~Week02_FullscreenFrag();

protected:

	virtual bool onCreate();
	virtual void Update(float a_deltaTime);
	virtual void Draw();
	virtual void Destroy();

	
	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __Week02_FullscreenFrag_H_