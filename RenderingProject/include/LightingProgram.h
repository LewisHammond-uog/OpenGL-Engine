#ifndef __LIGHTINGPROGRAM_H__
#define __LIGHTINGPROGRAM_H__

#include "ShaderProgram.h"
#include "Material.h"

#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 2

//Forward Declerations
class BaseLight;
class DirectionalLight;
class PointLight;
class SpotLight;

class LightingProgram : public ShaderProgram
{
public:

	LightingProgram() = default;
	bool Initialise() override;

	void SetWorldViewPoint(const glm::mat4 a_worldViewPoint) const;
	void SetDiffuseTextureUnit(const unsigned int a_textureUnit) const;
	void SetSpecularPowerTextureUnit(const unsigned int a_textureUnit) const;
	void SetDirectionalLight(const DirectionalLight& a_light);
	void SetPointLights(unsigned int a_numLights, PointLight* a_pLights[]);
	void SetSpotLights(unsigned int a_numLights, SpotLight* a_pLights[]);
	void SetCameraLocalPos(const glm::vec3& a_cameraLocalPosition) const;
	void SetMaterial(const Material& a_material) const;

private:

	void InitalizePointLightUniformLocations();
	void InitalizeSpotLightUniformLocations();

	GLint m_worldViewPointLocation; //Uniform Location for WVP Matrix
	GLint m_diffuseSamplerLocation; //Uniform Location for the diffuse texture sampler
	GLint m_specularSamplerLocation; //Uniform location for the specular power texture sampler
	GLint m_cameraPositionLocation; //Uniform location for the camera location
	GLint m_numPointLightsLocation; //Uniform location for the number of point lights
	GLint m_numSpotLightsLocation; //Uniform location for the number of spot lights

	//-- Structs for use in Shader --//
	//Struct for the location of material uniform locations in the shader
	struct ShaderMaterialUnifomLocations
	{
		GLuint ambientColour;
		GLuint diffuseColour;
		GLuint specularColour;
	} materialLocation;

	//Struct for the location of directional light uniforms in the shader
	struct ShaderDirectionalLightUniformLocations
	{
		GLuint colour;
		GLuint ambientIntensity;
		GLuint direction;
		GLuint diffuseIntensity;
	} dirLightLocation;

	//Struct for locations of point light uniforms in the shader
	struct ShaderPointLightUniformLocations
	{
		GLuint colour;
		GLuint ambientIntensity;
		GLuint diffuseIntensity;
		GLuint position;

		struct AttenuationUniformLocations
		{
			GLuint constant;
			GLuint linear;
			GLuint exponential;
		} attenuation;
	} pointLightsLocation[MAX_POINT_LIGHTS];

	//Struct for the locals of the spot light uniforms in the shader
	struct ShaderSpotLightUniformLocations : public ShaderPointLightUniformLocations
	{
		GLuint direction;
		GLuint cutoff;
	};

	ShaderSpotLightUniformLocations spotLightLocations[MAX_SPOT_LIGHTS];
};

#endif // __LIGHTINGPROGRAM_H__
