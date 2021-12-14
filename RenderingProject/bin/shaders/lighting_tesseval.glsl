#version 410 core

layout( triangles, equal_spacing, ccw ) in;

//Inputs from TessControl Shader
in vec2 tcTexCoord[];
in vec3 tcNormal[];
in vec3 tcLocalPos[];
in vec3 tcTangent[];
in vec4 tcFragPosLight[];
in vec4 tcShadowCoord[];

//Outputs to Fragment Shader
out vec2 vTexCoord;
out vec3 vNormal;
out vec3 vLocalPos;
out vec3 vTangent;
out vec4 vFragPosLight;
out vec4 vShadowCoord;


vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

vec4 interpolate4D(vec4 v0, vec4 v1, vec4 v2)
{
	return vec4(gl_TessCoord.x) * v0 + vec4(gl_TessCoord.y) * v1 + vec4(gl_TessCoord.z) * v2;
}


void main(){
	
	// Interpolate the attributes of the output vertex using the barycentric coordinates
    vTexCoord = interpolate2D(tcTexCoord[0], tcTexCoord[1], tcTexCoord[2]);
    vNormal = interpolate3D(tcNormal[0], tcNormal[1], tcNormal[2]);
    vNormal = normalize(vNormal);
    vLocalPos = interpolate3D(tcLocalPos[0], tcLocalPos[1], tcLocalPos[2]);
    vTangent = interpolate3D(tcTangent[0], tcTangent[1], tcTangent[2]);
	vFragPosLight = interpolate4D(tcFragPosLight[0], tcFragPosLight[1], tcFragPosLight[2]);
	vShadowCoord = interpolate4D(tcShadowCoord[0], tcShadowCoord[1], tcShadowCoord[2]);

	vec4 p0 = gl_in[0].gl_Position;
	vec4 p1 = gl_in[1].gl_Position;
	vec4 p2 = gl_in[2].gl_Position;
	vec3 p = gl_TessCoord.xyz;
	gl_Position = p0 * p.x + p1 * p.y + p2 *p.z;
}