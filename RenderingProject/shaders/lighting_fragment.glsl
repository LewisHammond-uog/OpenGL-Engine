#version 330

in vec2 vTexCoord;

out vec4 fragColour;

struct BaseLight{
	vec3 Colour;
	float AmbientIntensity;
};

struct Material{
	vec3 AmbientColour;
};

uniform BaseLight uLight;
uniform Material uMaterial;
uniform sampler2D uSampler;


void main(){
	
	fragColour = texture2D(uSampler, vTexCoord.xy) *
				vec4(uMaterial.AmbientColour, 1.0f) *
				vec4(uLight.Colour, 1.0f) *
				uLight.AmbientIntensity;
}