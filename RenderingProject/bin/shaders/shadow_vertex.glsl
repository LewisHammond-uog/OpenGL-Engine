#version 330

layout (location = 0) in vec4 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;

uniform mat4 uLightViewPoint;

void main(){
	gl_Position = uLightViewPoint * vec4(Position.xyz, 1.0);
}