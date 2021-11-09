#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include <glm/glm.hpp>

//Forward declerations
class Texture;

class Material
{
public:
	Material() = default;
	~Material();

	glm::vec3 m_ambientColour = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 m_diffuseColour = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 m_specularColour = glm::vec3(0.f, 0.f, 0.f);

	Texture* m_pDiffuseTexture = nullptr; //Texture that stores the colour values
	Texture* m_pSpecularTexture = nullptr; // Texture that stores the specular exponent
};

#endif // __MATERIAL_H__
