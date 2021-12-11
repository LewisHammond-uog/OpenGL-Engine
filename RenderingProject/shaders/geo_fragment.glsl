#version 330

in vec4 vColour;
in vec2 vUV;

out vec4 FragColor;

uniform sampler2D TextureSampler;

void main(){
	//Output the colour of the particle multiplied by the alpha of the texture
	FragColor = vColour * texture2D(TextureSampler, vUV).a;
}