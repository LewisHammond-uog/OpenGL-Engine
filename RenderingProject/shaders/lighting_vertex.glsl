#version 330

layout (location = 0) in vec4 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;

out vec2 vTexCoord;
out vec3 vNormal;
out vec3 vLocalPos;
out vec3 vTangent;
out vec4 vFragPosLight;
out vec4 vShadowCoord;

uniform mat4 uWorldViewPoint;
uniform mat4 uLightViewPoint;

const mat4 SHADOW_BIAS_MATRIX = mat4(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
);


void main(){
	vTexCoord = TexCoord;
	vNormal = Normal;
	vLocalPos = Position.xyz;
	vTangent = Tangent;
	vFragPosLight = uLightViewPoint * vec4(vLocalPos, 0);
	vShadowCoord = SHADOW_BIAS_MATRIX * uLightViewPoint * Position;
	gl_Position = uWorldViewPoint * vec4(Position.xyz, 1.0);
}