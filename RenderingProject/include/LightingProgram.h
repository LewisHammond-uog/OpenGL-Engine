#ifndef __LIGHTINGPROGRAM_H__
#define __LIGHTINGPROGRAM_H__

#include "ShaderProgram.h"
#include "Material.h"

//Forward Declerations
struct BaseLight
{
	glm::vec3 m_lightColour;
	float m_lightIntensity;

	BaseLight()
	{
		m_lightColour = glm::vec3(1.0f, 1.0f, 1.0f);
		m_lightIntensity = 0.f;
	}
};

class LightingProgram : public ShaderProgram
{
public:

	LightingProgram() = default;
	bool Initialise() override;

	void SetWorldViewPoint(const glm::mat4 a_worldViewPoint);
	void SetTextureUnit(const unsigned int a_textureUnit);
	void SetLight(const BaseLight& a_light);
	void SetMaterial(const Material& a_material);

private:

	GLint m_worldViewPointLocation;
	GLint m_samplerLocation;
	GLint m_lightColourLocation;
	GLint m_lightAmbitentIntensityLocation;
	GLint m_materialAmbientColourLocation;

};

#endif // __LIGHTINGPROGRAM_H__
