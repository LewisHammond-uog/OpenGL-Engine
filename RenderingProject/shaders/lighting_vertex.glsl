#version 330

layout (location = 0) in vec4 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;

out vec2 vTexCoord;
out vec3 vNormal;
out vec3 vLocalPos;

uniform mat4 uWorldViewPoint;


void main(){
	vTexCoord = TexCoord;
	vNormal = Normal;
	vLocalPos = Position.xyz;
	gl_Position = uWorldViewPoint * vec4(Position.xyz, 1.0);
}