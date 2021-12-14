#ifndef __BASELIGHT_H__
#define __BASELIGHT_H__

#include <glm/glm.hpp>

//Structure to describe the attenuation properties of a light
struct LightAttenuation
{
public:
	float m_fConstant = 1.0f;
	float m_fLinear = 0.0f;
	float m_fExponential = 0.0f;
};

class BaseLight
{

public:
	BaseLight()
	{
		m_v3LightColour = glm::vec3(1.0f, 1.0f, 1.0f);
		m_fAmbientIntensity = 0.f;
	}

	glm::vec3 m_v3LightColour;
	float m_fAmbientIntensity;
	float m_fDiffuseIntensity = 0.0f;
};

#endif // __BASELIGHT_H__
