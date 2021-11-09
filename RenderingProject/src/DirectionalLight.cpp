#include "DirectionalLight.h"

/// <summary>
/// Calculate the direction of this light in local space given a matrix
/// </summary>
/// <param name="a_world"></param>
void DirectionalLight::CalculateLocalDirection(const glm::mat4& a_world)
{
	//Initalise the top left corner of a matrix - translation does not effect the direction vector
	const glm::mat3 worldMatrix = glm::mat3(a_world);

	//Inverse local-to-world transformation using transpose
	const glm::mat3 worldToLocalMat = glm::transpose(worldMatrix);

	//Get the local direction by multiplying by the world to local matrix and normalize
	m_localDirection = worldToLocalMat * m_worldDirection;
	m_localDirection = glm::normalize(m_localDirection);
}

/// <summary>
/// Get the direction of the light in local space
/// </summary>
/// <returns></returns>
glm::vec3 DirectionalLight::GetLocalDirection() const
{
	return m_localDirection;
}
