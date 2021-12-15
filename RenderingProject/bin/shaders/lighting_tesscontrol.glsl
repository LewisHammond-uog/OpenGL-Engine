#version 410 core

layout ( vertices = 3 ) out;

//Inputs from vertex shader
in vec2 iTexCoord[];
in vec3 iNormal[];
in vec3 iLocalPos[];
in vec3 iTangent[];
in vec4 iFragPosLight[];
in vec4 iShadowCoord[];

//Outputs from TessControl Shader
out vec2 tcTexCoord[];
out vec3 tcNormal[];
out vec3 tcLocalPos[];
out vec3 tcTangent[];
out vec4 tcFragPosLight[];
out vec4 tcShadowCoord[];

uniform vec3 uCameraLocalPos;

//Calculate the amount of tesselation based on distance to the camera
float GetTessLevel(float Distance)
{
    if (Distance <= 20.0) {
        return 5.0;
    }else if(Distance <= 10.0){
        return 2.0;
    }
    else {
        return 1.0;
    }
}


void main(){

	// Set the control points of the output patch
    tcTexCoord[gl_InvocationID] = iTexCoord[gl_InvocationID];
    tcNormal[gl_InvocationID] = iNormal[gl_InvocationID];
    tcLocalPos[gl_InvocationID] = iLocalPos[gl_InvocationID];
    tcTangent[gl_InvocationID] = iTangent[gl_InvocationID];
    tcFragPosLight[gl_InvocationID] = iFragPosLight[gl_InvocationID];
    tcShadowCoord[gl_InvocationID] = iShadowCoord[gl_InvocationID];


	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;


	 // Calculate the distance from the camera to the  control points
    float EyeToVertexDistance = distance(uCameraLocalPos, gl_in[gl_InvocationID].gl_Position.xyz);
    float TessLevel = GetTessLevel(EyeToVertexDistance);

    gl_TessLevelOuter[0] = TessLevel;
    gl_TessLevelOuter[1] = TessLevel;
    gl_TessLevelOuter[2] = TessLevel;
    gl_TessLevelInner[0] = gl_TessLevelOuter[2];
}