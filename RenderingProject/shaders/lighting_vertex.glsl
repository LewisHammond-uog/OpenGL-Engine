#version 330

layout (location = 0)  in vec4 Position;
layout (location = 1) in vec2 TexCoord;

out vec2 vTexCoord;

uniform mat4 uWorldViewPoint;


void main(){
	vTexCoord = TexCoord;
	gl_Position = uWorldViewPoint * vec4(Position.xyz, 1.0);
}