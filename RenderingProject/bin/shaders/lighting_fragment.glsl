#version 330

//Inputs from TE Shader OR Vertex Shader (if no tessellation)
in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vLocalPos;
in vec3 vTangent;
in vec4 vFragPosLight;
in vec4 vShadowCoord;

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
uniform sampler2D uNormalSampler;
uniform sampler2D uShadowSampler;
uniform Material uMaterial;

uniform vec3 uCameraLocalPos;

const float SHADOW_BIAS = 0.002f;

const vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

float CalculateShadow(){
	float shadowFactor = 0;
	for(int i = 0; i < 4; ++i){
		if( texture(uShadowSampler, vShadowCoord.xy + poissonDisk[i]/700).z  < vShadowCoord.z - SHADOW_BIAS){
			shadowFactor += 0.2;
		}
	}

	return shadowFactor;
}

vec4 CalculateLightInternal(BaseLight light, vec3 lightDirection, vec3 normal, float shadowFactor){

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

	return (ambientColour + shadowFactor * (diffuseColour + specularColour));
}

vec4 CalculateDirectionalLight(vec3 normal)
{
	float shadowFactor = 1 - CalculateShadow();
	return CalculateLightInternal(uDirectionalLight.base, uDirectionalLight.Direction, normal, shadowFactor);
}

vec4 CalculatePointLight(PointLight light, vec3 normal)
{
	vec3 LightDirection = light.localPos - vLocalPos;
	float Distance = length(LightDirection);
	LightDirection = normalize(LightDirection);

	vec4 Color = CalculateLightInternal(light.base, LightDirection, normal, 1);
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

vec3 CalcuateBumpedNormal()
{
	//Normalize the tanget and normal
	vec3 Normal = normalize(vNormal);
	vec3 Tangent = normalize(vTangent);
	//Get the length of the projection along the normal vector - Gramm-Schmidt
	Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
	//Get the Bi Tangent
	vec3 Bitangent = cross(Tangent, Normal);
	//Sample the normal from the Normal Map - this is stored in 0 to 1 we need it in -1 to 1
	//bring it back with f(x) = 2 * x - 1
	vec3 BumpMapNormal = texture(uNormalSampler, vTexCoord).xyz;
	BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
	//Transform the normal back to world space
	vec3 NewNormal;
	mat3 TBN = mat3(Tangent, Bitangent, Normal);
	NewNormal = TBN * BumpMapNormal;
	NewNormal = normalize(NewNormal);
	return NewNormal;
}

void main(){
	
	vec3 normal = CalcuateBumpedNormal();
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