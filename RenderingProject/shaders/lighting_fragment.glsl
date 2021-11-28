#version 330

in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vLocalPos;

out vec4 fragColour;

struct BaseLight{
	vec3 Colour;
	float AmbientIntensity;
	float DiffuseIntensity;
};

struct DirectionalLight{
	BaseLight base;
	vec3 Direction;
};

struct Attenuation{
	float constant;
	float linear;
	float exponential;
};

struct PointLight{
	BaseLight base;
	vec3 localPos;
	Attenuation atten;
};

struct SpotLight {
	PointLight base; //Point light is base member as we use all of its properties
	vec3 direction;
	float cutoff; //COS of the cutoff rather than the angle itself
};

struct Material{
	vec3 AmbientColour;
	vec3 DiffuseColour;
	vec3 SpecularColour;
};

//Lights
const int MAX_POINT_LIGHTS = 4;
const int MAX_SPOT_LIGHTS = 2;
uniform DirectionalLight uDirectionalLight;
//Point Lights
uniform int uPointLightCount; //Number of point lights passed 
uniform PointLight uPointLights[MAX_POINT_LIGHTS];
//Spot Lights
uniform int uSpotLightCount;
uniform SpotLight uSpotLights[MAX_SPOT_LIGHTS];

//Textures/Materials
uniform sampler2D uDiffuseSampler;
uniform sampler2D uSpecularSampler;
uniform Material uMaterial;

uniform vec3 uCameraLocalPos;

vec4 CalculateLightInternal(BaseLight light, vec3 lightDirection, vec3 normal){

	//Calculate ambient lighting based from properties of diretional light
	vec4 ambientColour =	vec4(light.Colour, 1.f) *
							vec4(uMaterial.AmbientColour, 1.f) *
							light.AmbientIntensity;

	//Calculate how much diffuse lighting we need base on the dot product of the normal and the direction from
	//the light to the eye - this is the cos of theta
	float diffuseFactor = dot(normalize(vNormal), -lightDirection);

	//If the diffuse Factor is greater than zero then there is some lighting - calculate the lighting
	//based on the colour of the material and light and the intenstity of the light
	vec4 diffuseColour = vec4(0.f);
	vec4 specularColour = vec4(0.f);
	if(diffuseFactor > 0)
	{
		diffuseColour = vec4(light.Colour, 1.0f) * vec4(uMaterial.DiffuseColour, 1.0f) * 
						light.DiffuseIntensity * diffuseFactor;


		//--Specular--//
		vec3 pixelToCamera = normalize(uCameraLocalPos - vLocalPos); //Vector from pixel (texel/fragment) to camera
		vec3 lightReflect = normalize(reflect(lightDirection, normal)); //Vector of the light reflected around the normal
		float specularFactor = dot(pixelToCamera, lightReflect); //Amount of specular to how much the light and camera vectors alighn
		if(specularFactor > 0){
			float specularPower = texture2D(uSpecularSampler, vTexCoord).r * 255.0f; //Read the amount of specular power from the specular texture
			specularFactor = pow(specularFactor, specularPower);
			
			//Create the specular colour from the camera colour and the materials specular colour * (the specular factor * specular power)
			specularColour =	vec4(light.Colour, 1.0f) *
								vec4(uMaterial.SpecularColour, 1.0f) *
								specularFactor;
		}
	}

	return (ambientColour + diffuseColour + specularColour);
}

vec4 CalculateDirectionalLight(vec3 normal)
{
	return CalculateLightInternal(uDirectionalLight.base, uDirectionalLight.Direction, normal);
}

vec4 CalculatePointLight(PointLight light, vec3 normal)
{
	vec3 LightDirection = light.localPos - vLocalPos;
	float Distance = length(LightDirection);
	LightDirection = normalize(LightDirection);

	vec4 Color = CalculateLightInternal(light.base, LightDirection, normal);
	float Attenuation = light.atten.constant +
		light.atten.linear * Distance +
		light.atten.exponential * Distance * Distance;

	return Color / Attenuation;
}

vec4 CalculateSpotLight(SpotLight light, vec3 normal) 
{
	//Get the vector from the spot light to the pixel
	vec3 lightToPixel = normalize(vLocalPos - light.base.localPos);
	//Calculate the COS of the angle that we are at compare to the spot lights direction
	float spotFactor = dot(lightToPixel, light.direction);

	//Check if we are within the cone of the spot light by comparing if the spot factor (COS of angle from light to pixel and light direction)
	//is greater than the cutoff value (which is the COS of the light direction and it's the outer angle vector)
	if (spotFactor > light.cutoff) {
		//Calculate as if we are a point light then determine the intensity base on the cuttoff
		vec4 colour = CalculatePointLight(light.base, normal);
		float intensity = (1.0 - (1.0 - spotFactor) / (1.0 - light.cutoff));
		return colour * intensity;
	}
	else {
		//Not in cone - zero light
		return vec4(0, 0, 0, 0);
	}
}

void main(){
	
	vec3 normal = normalize(vNormal);
	vec4 totalLight = CalculateDirectionalLight(normal);

	//Loop all of the point lights and add up the lighting
	for(int i = 0; i < uPointLightCount; i++){
		totalLight += CalculatePointLight(uPointLights[i], normal);
	}

	//Loop all of the spot lights and add up the lighting
	for (int i = 0; i < uSpotLightCount; i++) {
		totalLight += CalculateSpotLight(uSpotLights[i], normal);
	}

	//Final Colour is the colour of the texture multiplied by the light colours
	fragColour = texture2D(uDiffuseSampler, vTexCoord.xy) * totalLight;
}