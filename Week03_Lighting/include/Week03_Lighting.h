#ifndef __Week03_Lighting_H_
#define __Week03_Lighting_H_

#include "Application.h"
#include <glm/glm.hpp>

// Derived application class that wraps up all globals neatly
class Week03_Lighting : public Application
{
public:

	Week03_Lighting();
	virtual ~Week03_Lighting();

protected:

	virtual bool onCreate();
	virtual void Update(float a_deltaTime);
	virtual void Draw();
	virtual void Destroy();

	
	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __Week03_Lighting_H_