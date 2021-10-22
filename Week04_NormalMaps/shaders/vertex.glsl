#version 150

in vec4 Position;
in vec4 Colour;
in vec4 Normal;
in vec4 Tangent;
in vec4 BiTangent;
in vec2 Tex1;

out vec4 vPos;
out vec4 vNormal;
out vec4 vColour; 
out vec2 vUV;
out vec4 vLightDir;
out vec4 vViewDir;

uniform mat4 ProjectionView; 
uniform mat4 ViewMatrix;
uniform mat4 Model;
uniform mat4 NormalMatrix;

uniform vec4 cameraPosition;
uniform vec4 lightDirection;

uniform int useNormalMap = 1;

void main() 
{ 
	vPos = ViewMatrix * Model * Position;
	vNormal = NormalMatrix * Normal;

	//Multiply our Normal, Tangent and BiTangent in to view space and construct a matrix
	vec4 N = NormalMatrix * Normal; 
	vec4 T = NormalMatrix * Tangent; 
	vec4 B = NormalMatrix * BiTangent; 
	mat3 TBN = transpose(mat3(T.xzy, B.xyz, N.xyz));

	//Get our eye direction
	vec4 E = normalize((ViewMatrix * Model * cameraPosition) - vPos);

	//If we are using normal maps transform the view direction and light direction in to normal/tangent/texture space
	if(useNormalMap == 1){
		vViewDir = vec4(TBN * E.xyz, 0.0);
		vLightDir = vec4(TBN * (ViewMatrix * Model * lightDirection).xyz, 0.0);
	}else{
		vViewDir = vec4(E.xyz, 0.0);
		vLightDir = vec4((ViewMatrix * Model * lightDirection).xyz, 0.0);
	}

	vColour = Colour;
	vUV = Tex1;
	gl_Position = ProjectionView * Model * Position;
}