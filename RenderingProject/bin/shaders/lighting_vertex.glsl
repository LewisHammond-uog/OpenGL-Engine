#version 330

layout (location = 0) in vec4 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;

//Outputs to TC Shader OR Vertex Shader (if no tessellation)
out vec2 iTexCoord;
out vec3 iNormal;
out vec3 iLocalPos;
out vec3 iTangent;
out vec4 iFragPosLight;
out vec4 iShadowCoord;

uniform mat4 uWorldViewPoint;
uniform mat4 uLightViewPoint;

const mat4 SHADOW_BIAS_MATRIX = mat4(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
);


void main(){
	
	//Transfer of Data
	iTexCoord = TexCoord;
	iNormal = Normal;
	iLocalPos = Position.xyz;
	iTangent = Tangent;

	//Calculate the position of this fragment in light space
	iFragPosLight = uLightViewPoint * vec4(iLocalPos, 0);
	//Get the coordinate of the shadow map in light space
	iShadowCoord = SHADOW_BIAS_MATRIX * uLightViewPoint * Position;
	//Move the fragment to camera space
	gl_Position = uWorldViewPoint * vec4(Position.xyz, 1.0);
}