#ifndef __BASELIGHT_H__
#define __BASELIGHT_H__

#include <glm/glm.hpp>

//Structure to describe the attenuation properties of a light
struct LightAttenuation
{
public:
	float m_constant = 1.0f;
	float m_linear = 0.0f;
	float m_exponential = 0.0f;
};

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
	float m_diffuseIntensity = 0.0f;
};

#endif // __BASELIGHT_H__
