#version 330

layout (location = 0)  in vec4 Position;
layout (location = 1) in vec2 TexCoord;
in vec4 Colour;
in vec4 Normal;

out vec4 vPosition;
out vec4 vColour;
out vec4 vNormal;
out vec2 vTexCoord;

uniform mat4 ProjectionView;
uniform mat4 ModelMatrix;
uniform mat4 NormalMatrix;

void main(){

	vColour = Colour;
	vNormal = NormalMatrix * Normal;
	vPosition = ModelMatrix * Position;
	vTexCoord = TexCoord;
	gl_Position = ProjectionView * ModelMatrix * Position;
}