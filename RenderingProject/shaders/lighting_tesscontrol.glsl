#version 410 core

layout ( vertices = 3 ) out;

in vec2 iTexCoord[];
in vec3 iNormal[];
in vec3 iLocalPos[];
in vec3 iTangent[];
in vec4 iFragPosLight[];
in vec4 iShadowCoord[];

out vec2 tcTexCoord[];
out vec3 tcNormal[];
out vec3 tcLocalPos[];
out vec3 tcTangent[];
out vec4 tcFragPosLight[];
out vec4 tcShadowCoord[];


void main(){

	   // Set the control points of the output patch
    tcTexCoord[gl_InvocationID] = iTexCoord[gl_InvocationID];
    tcNormal[gl_InvocationID] = iNormal[gl_InvocationID];
    tcLocalPos[gl_InvocationID] = iLocalPos[gl_InvocationID];
    tcTangent[gl_InvocationID] = iTangent[gl_InvocationID];
    tcFragPosLight[gl_InvocationID] = iFragPosLight[gl_InvocationID];
    tcShadowCoord[gl_InvocationID] = iShadowCoord[gl_InvocationID];



	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	gl_TessLevelOuter[0] = 5.0f;
	gl_TessLevelOuter[1] = 5.0f;
	gl_TessLevelOuter[2] = 5.0f;
	gl_TessLevelOuter[0] = 5.0f;
}