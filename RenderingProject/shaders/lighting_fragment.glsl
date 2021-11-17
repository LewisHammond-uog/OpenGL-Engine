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

struct Material{
	vec3 AmbientColour;
	vec3 DiffuseColour;
	vec3 SpecularColour;
};

//Lights
const int MAX_POINT_LIGHTS = 4;
uniform DirectionalLight uDirectionalLight;
uniform int uPointLightCount; //Number of point lights passed 
uniform PointLight uPointLights[MAX_POINT_LIGHTS];

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
	vec4 diffuseColour = vec4(1.f);
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

vec4 CalculatePointLight(int index, vec3 normal)
{
	vec3 LightDirection =  uPointLights[index].localPos - vLocalPos;
	float Distance = length(LightDirection);
	LightDirection = normalize(LightDirection);

	vec4 Color = CalculateLightInternal(uPointLights[index].base, LightDirection, normal);
	float Attenuation = uPointLights[index].atten.constant +
		uPointLights[index].atten.linear * Distance +
		uPointLights[index].atten.exponential * Distance * Distance;

	return Color / Attenuation;
}

void main(){
	
	vec3 normal = normalize(vNormal);
	vec4 totalLight = vec4(0);

	//Loop all of the point lights and add up the lighting
	for(int i = 0; i < uPointLightCount; i++){
		totalLight += CalculatePointLight(i, normal);
	}

	//Final Colour is the colour of the texture multiplied by the light colours
	fragColour = texture2D(uDiffuseSampler, vTexCoord.xy) * totalLight;
}