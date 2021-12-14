#include "DirectionalLight.h"

#include "WorldTransform.h"

/// <summary>
/// Calculate the direction of this light in local space given a matrix
/// </summary>
/// <param name="a_rWorld"></param>
void DirectionalLight::CalculateLocalDirection(WorldTransform& a_rWorld)
{
	m_v3LocalDirection = a_rWorld.WorldDirToLocalDir(m_v3WorldDirection);
}

/// <summary>
/// Get the direction of the light in local space
/// </summary>
/// <returns></returns>
glm::vec3 DirectionalLight::GetLocalDirection() const
{
	return m_v3LocalDirection;
}
