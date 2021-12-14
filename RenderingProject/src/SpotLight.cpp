#include "SpotLight.h"

//Project includes
#include "WorldTransform.h"

/// <summary>
/// Calculate direction and position of the light in local space
/// </summary>
/// <param name="a_pWorldTransform"></param>
void SpotLight::CalculateLocalDirectionAndPosition(WorldTransform& a_pWorldTransform)
{
	CalculateLocalPosition(a_pWorldTransform);
	m_v3LocalDirection = a_pWorldTransform.WorldDirToLocalDir(m_v3WorldDirection);
}
