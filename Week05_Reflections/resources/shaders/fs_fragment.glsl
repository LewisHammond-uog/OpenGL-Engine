#version 150

in vec2 uv;

out vec4 FragColour;

uniform sampler2D DiffuseTexture;
uniform vec2 ScreenSize;

void main()
{
	vec2 reflectedUV = gl_FragCoord.xy / ScreenSize;
	FragColour = texture(DiffuseTexture, reflectedUV);
}
