#version 330

in vec4 vPosition;
in vec4 vColour;
in vec2 vTexCoord;

out vec4 fragColour;

uniform vec4 CameraPosition;
uniform vec3 LightDirection;
uniform vec4 LightColour;

uniform sampler2D DiffuseTexture;

void main(){
	vec4 diffuse = texture(DiffuseTexture, vTexCoord);
	fragColour = vec4(1,1,1,1);
}