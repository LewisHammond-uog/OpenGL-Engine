#ifndef __SPOT_LIGHT_H__
#define __SPOT_LIGHT_H__

#include "PointLight.h"

#include "glm/vec3.hpp"

//Forward Declerations
class WorldTransform;

class SpotLight : public PointLight
 {
public:

	glm::vec3 m_worldDirection = glm::vec3(0.0f, 0.f, 0.f);
	float m_cutoff = 0.f;

	void CalculateLocalDirectionAndPosition(WorldTransform& a_pWorldTransform);

	const glm::vec3 GetLocalDirection() const { return m_localDirection; }

 private:
	 glm::vec3 m_localDirection = glm::vec3(0.0f, 0.f, 0.f);

};

#endif //__SPOT_LIGHT_H__