#ifndef __BASELIGHT_H__
#define __BASELIGHT_H__

#include <glm/glm.hpp>

class BaseLight
{

public:
	BaseLight()
	{
		m_lightColour = glm::vec3(1.0f, 1.0f, 1.0f);
		m_ambientIntensity = 0.f;
	}

	glm::vec3 m_lightColour;
	float m_ambientIntensity;
};

#endif // __BASELIGHT_H__
