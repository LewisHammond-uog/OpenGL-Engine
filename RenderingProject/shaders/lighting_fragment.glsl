#version 330

in vec2 vTexCoord;
in vec3 vNormal;

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
};

uniform DirectionalLight uDirectionalLight;
uniform Material uMaterial;
uniform sampler2D uSampler;


void main(){
	
	//Calculate ambient lighting based from properties of diretional light
	vec4 ambientColour =	vec4(uDirectionalLight.Colour, 1.f) *
							vec4(uMaterial.AmbientColour, 1.f) *
							uDirectionalLight.AmbientIntensity;

	//Calculate how much diffuse lighting we need base on the dot product of the normal and the direction from
	//the light to the eye - this is the cos of theta
	float diffuseFactor = dot(normalize(vNormal), -uDirectionalLight.Direction);

	//If the diffuse Factor is greater than zero then there is some lighting - calculate the lighting
	//based on the colour of the material and light and the intenstity of the light
	vec4 diffuseColour = vec4(0.f);
	if(diffuseFactor > 0)
	{
		diffuseColour = vec4(uDirectionalLight.Colour, 1.0f) * vec4(uMaterial.DiffuseColour, 1.0f) * 
						uDirectionalLight.DiffuseIntensity * diffuseFactor;
	}

	//Final Colour is the colour of the texture multiplied by the light colours
	fragColour =	texture2D(uSampler, vTexCoord.xy) *
					(ambientColour + diffuseColour);
}