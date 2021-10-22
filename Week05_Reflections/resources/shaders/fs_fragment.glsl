#version 150

in vec2 uv;

out vec4 FragColour;

uniform sampler2D DiffuseTexture;

void main()
{
	FragColour = texture(DiffuseTexture, uv);
}
