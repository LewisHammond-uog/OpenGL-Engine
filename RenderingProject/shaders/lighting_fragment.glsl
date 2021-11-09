#version 330

in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vLocalPos;

out vec4 fragColour;

struct DirectionalLight{
	vec3 Colour;
	float AmbientIntensity;
	float DiffuseIntensity;
	vec3 Direction;
};

struct Material{
	vec3 AmbientColour;
	vec3 DiffuseColour;
	vec3 SpecularColour;
};

uniform sampler2D uDiffuseSampler;
uniform sampler2D uSpecularSampler;
uniform DirectionalLight uDirectionalLight;
uniform Material uMaterial;
uniform vec3 uCameraLocalPos;


void main(){
	
	//Calculate ambient lighting based from properties of diretional light
	vec4 ambientColour =	vec4(uDirectionalLight.Colour, 1.f) *
							vec4(uMaterial.AmbientColour, 1.f) *
							uDirectionalLight.AmbientIntensity;

	vec3 normal = normalize(vNormal);
	//Calculate how much diffuse lighting we need base on the dot product of the normal and the direction from
	//the light to the eye - this is the cos of theta
	float diffuseFactor = dot(normalize(vNormal), -uDirectionalLight.Direction);

	//If the diffuse Factor is greater than zero then there is some lighting - calculate the lighting
	//based on the colour of the material and light and the intenstity of the light
	vec4 diffuseColour = vec4(0.f);
	vec4 specularColour = vec4(0.f);
	if(diffuseFactor > 0)
	{
		diffuseColour = vec4(uDirectionalLight.Colour, 1.0f) * vec4(uMaterial.DiffuseColour, 1.0f) * 
						uDirectionalLight.DiffuseIntensity * diffuseFactor;


		//--Specular--//
		vec3 pixelToCamera = normalize(uCameraLocalPos - vLocalPos); //Vector from pixel (texel/fragment) to camera
		vec3 lightReflect = normalize(reflect(uDirectionalLight.Direction, normal)); //Vector of the light reflected around the normal
		float specularFactor = dot(pixelToCamera, lightReflect); //Amount of specular to how much the light and camera vectors alighn
		if(specularFactor > 0){
			float specularPower = texture2D(uSpecularSampler, vTexCoord).r * 255.0f; //Read the amount of specular power from the specular texture
			specularFactor = pow(specularFactor, specularPower);
			
			//Create the specular colour from the camera colour and the materials specular colour * (the specular factor * specular power)
			specularColour =	vec4(uDirectionalLight.Colour, 1.0f) *
								vec4(uMaterial.SpecularColour, 1.0f) *
								specularFactor;
		}
	}

	//Final Colour is the colour of the texture multiplied by the light colours
	fragColour =	texture2D(uDiffuseSampler, vTexCoord.xy) *
					clamp((ambientColour + diffuseColour + specularColour), 0, 1);
}