#ifndef __POINTLIGHT_H__
#define __POINTLIGHT_H__

#include "BaseLight.h"

//Forward Declares
class WorldTransform;

class PointLight : public BaseLight
{
public:
	glm::vec3 m_v3WorldPosition = glm::vec3(0, 0, 0);
	LightAttenuation m_attenuation;

	void CalculateLocalPosition(WorldTransform& a_worldTransform);

	const glm::vec3& GetLocalPosition() const;

private:
	glm::vec3 m_v3LocalPosition;

};


#endif // __POINTLIGHT_H__
