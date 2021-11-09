#ifndef __LIGHTINGPROGRAM_H__
#define __LIGHTINGPROGRAM_H__

#include "ShaderProgram.h"
#include "Material.h"

//Forward Declerations
class BaseLight;
class DirectionalLight;

class LightingProgram : public ShaderProgram
{
public:

	LightingProgram() = default;
	bool Initialise() override;

	void SetWorldViewPoint(const glm::mat4 a_worldViewPoint);
	void SetTextureUnit(const unsigned int a_textureUnit);
	void SetDirectionalLight(const DirectionalLight& a_light);
	void SetMaterial(const Material& a_material);

private:

	GLint m_worldViewPointLocation;
	GLint m_samplerLocation;
	GLint m_lightColourLocation;
	GLint m_lightAmbitentIntensityLocation;
	GLint m_materialAmbientColourLocation;

	//-- Structs for use in Shader --//
	struct ShaderMaterialUnifomLocations
	{
		GLuint ambientColour;
		GLuint diffuseColour;
	} materialLocation;

	struct ShaderLightUniformLocations
	{
		GLuint colour;
		GLuint ambientIntensity;
		GLuint direction;
		GLuint diffuseIntensity;
	} dirLightLocation;
};

#endif // __LIGHTINGPROGRAM_H__
