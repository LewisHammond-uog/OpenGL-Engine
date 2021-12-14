#ifndef __SPOT_LIGHT_H__
#define __SPOT_LIGHT_H__

#include "PointLight.h"

#include "glm/vec3.hpp"

//Forward Declerations
class WorldTransform;

class SpotLight : public PointLight
 {
public:

	glm::vec3 m_v3WorldDirection = glm::vec3(0.0f, 0.f, 0.f);
	float m_fCutoff = 0.f;

	void CalculateLocalDirectionAndPosition(WorldTransform& a_pWorldTransform);

	glm::vec3 GetLocalDirection() const { return m_v3LocalDirection; }

 private:
	 glm::vec3 m_v3LocalDirection = glm::vec3(0.0f, 0.f, 0.f);

};

#endif //__SPOT_LIGHT_H__