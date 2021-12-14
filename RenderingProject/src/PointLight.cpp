#include "PointLight.h"

#include "WorldTransform.h"

/// <summary>
/// Calculate the locaal position of this light from a given world transform
/// </summary>
/// <param name="a_worldTransform">World position to generate local position from</param>
void PointLight::CalculateLocalPosition(WorldTransform& a_worldTransform)
{
	m_v3LocalPosition = a_worldTransform.WorldPosToLocalPos(m_v3WorldPosition);
}

const glm::vec3& PointLight::GetLocalPosition() const
{
	return m_v3LocalPosition;
}
