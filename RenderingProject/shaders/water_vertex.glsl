#version 330

layout (location = 0) in vec4 Position;

uniform mat4 uWorldViewPoint;

uniform float uTime;

const float PI = 3.1415926535897932384626433832795;
const float amplitude = 0.01;

//Generate the height for this vertex
float GenerateHeight(){
	float component1 = sin(2.0 * PI * uTime + (Position.x * 16.0)) * amplitude;
	float component2 = sin(2.0 * PI * uTime + (Position.y * Position.x * 8.0)) * amplitude;
	return component1 + component2;
}

void main(){
	float height = GenerateHeight();
	vec4 pos = vec4(Position.x, height, Position.z, 1.0);
	gl_Position = pos;
}