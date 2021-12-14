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

uniform vec3 uCameraLocalPos;

float GetTessLevel(float Distance0, float Distance1)
{
    float AvgDistance = (Distance0 + Distance1) / 2.0;

    if (AvgDistance <= 20.0) {
        return 10.0;
    }else if(AvgDistance <= 10.0){
        return 3.0;
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


	  // Calculate the distance from the camera to the three control points
    float EyeToVertexDistance0 = distance(uCameraLocalPos, gl_in[gl_InvocationID].gl_Position.xyz);
    float EyeToVertexDistance1 = distance(uCameraLocalPos, gl_in[gl_InvocationID].gl_Position.xyz);
    float EyeToVertexDistance2 = distance(uCameraLocalPos, gl_in[gl_InvocationID].gl_Position.xyz);

    gl_TessLevelOuter[0] = GetTessLevel(EyeToVertexDistance1, EyeToVertexDistance2);
    gl_TessLevelOuter[1] = GetTessLevel(EyeToVertexDistance2, EyeToVertexDistance0);
    gl_TessLevelOuter[2] = GetTessLevel(EyeToVertexDistance0, EyeToVertexDistance1);
    gl_TessLevelInner[0] = gl_TessLevelOuter[2];
}