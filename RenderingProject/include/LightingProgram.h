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

	void SetWorldViewPoint(const glm::mat4 a_worldViewPoint) const;
	void SetDiffuseTextureUnit(const unsigned int a_textureUnit) const;
	void SetSpecularPowerTextureUnit(const unsigned int a_textureUnit) const;
	void SetDirectionalLight(const DirectionalLight& a_light);
	void SetCameraLocalPos(const glm::vec3& a_cameraLocalPosition) const;
	void SetMaterial(const Material& a_material) const;

private:

	GLint m_worldViewPointLocation; //Uniform Location for WVP Matrix
	GLint m_diffuseSamplerLocation; //Uniform Location for the diffuse texture sampler
	GLint m_specularSamplerLocation; //Uniform location for the specular power texture sampler
	GLint m_cameraPositionLocation; //Uniform location for the camera location

	//-- Structs for use in Shader --//
	struct ShaderMaterialUnifomLocations
	{
		GLuint ambientColour;
		GLuint diffuseColour;
		GLuint specularColour;
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
