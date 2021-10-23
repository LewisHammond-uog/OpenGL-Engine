#version 150

in vec4 Position;
in vec4 Colour;
in vec4 Normal;
in vec2 Tex1;

out Vertex
{
	vec4 Pos;
	vec4 Normal;
	vec4 Colour; 
	vec2 UV;
	vec3 HalfDir;
	vec3 LightDir;
	vec4 ShadowCoord;
}vertex;

uniform mat4 ProjectionView; 
uniform mat4 Model;
uniform mat4 NormalMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ShadowProjectionView;

uniform vec4 cameraPosition;
uniform vec4 lightDirection;

const mat4 SHADOW_BIAS_MATRIX = mat4(
								0.5, 0.0, 0.0, 0.0,
								0.0, 0.5, 0.0, 0.0,
								0.0, 0.0, 0.5, 0.0,
								0.5, 0.5, 0.5, 1.0
								);

void main() 
{ 
	vertex.Pos = Model * Position;
	vertex.Normal = ViewMatrix * NormalMatrix * Normal;
	vertex.Colour = Colour;
	vertex.UV = Tex1;

	vertex.LightDir = (ViewMatrix * -lightDirection).xyz;

	vec3 eyeDir = (ViewMatrix * normalize( cameraPosition - vertex.Pos)).xyz;
	vertex.HalfDir = normalize(eyeDir + vertex.LightDir);

	/*
	There is a small additional step which needs to be performed here, that the Shadow Bias Matrix is used for.  
	If you remember screen space has the origin at the centre of the screen with the top left coordinate as (-1,-1) and the bottom right as (1,1).  This is the coordinate system our transformed point is in.  
	But texture space has (0,0) in the top left and (1,1) in the bottom right, this is the coordinate system our shadow map is stored in. 
	So we need to perform an additional transform on the x,y data to get the coordinate into texture space
	Once we have done this we use it to read from the texture in the fragment shader
	*/
	vertex.ShadowCoord = SHADOW_BIAS_MATRIX * ShadowProjectionView * Position;

	gl_Position = ProjectionView * Model * Position;
}