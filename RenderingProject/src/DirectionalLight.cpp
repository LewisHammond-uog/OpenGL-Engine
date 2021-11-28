#include "DirectionalLight.h"

#include "WorldTransform.h"

/// <summary>
/// Calculate the direction of this light in local space given a matrix
/// </summary>
/// <param name="a_world"></param>
void DirectionalLight::CalculateLocalDirection(WorldTransform& a_world)
{
	m_localDirection = a_world.WorldDirToLocalDir(m_worldDirection);
}

/// <summary>
/// Get the direction of the light in local space
/// </summary>
/// <returns></returns>
glm::vec3 DirectionalLight::GetLocalDirection() const
{
	return m_localDirection;
}
