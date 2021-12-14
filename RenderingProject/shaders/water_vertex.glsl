#version 330

layout (location = 0) in vec4 Position;

uniform mat4 uWorldViewPoint;

void main(){
	gl_Position = uWorldViewPoint * vec4(Position.xyz, 1.0);
}