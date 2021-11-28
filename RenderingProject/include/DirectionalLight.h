#ifndef __DIRECTIONALLIGHT_H__
#define __DIRECTIONALLIGHT_H__

#include "BaseLight.h"

class WorldTransform;

class DirectionalLight : public BaseLight
{
public:
	//Direction of the light in world space
	glm::vec3 m_worldDirection = glm::vec3(0.f, 0.f, 0.f);

	void CalculateLocalDirection(WorldTransform& a_world);

	glm::vec3 GetLocalDirection() const;

private:
	//Direction of the light in local space
	glm::vec3 m_localDirection = glm::vec3(0.0f, 0.f, 0.f);
};

#endif // __DIRECTIONALLIGHT_H__
