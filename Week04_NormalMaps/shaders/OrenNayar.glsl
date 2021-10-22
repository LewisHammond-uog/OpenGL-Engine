#version 150

#define PI 3.14159265

in vec4 vPos;
in vec4 vNormal;
in vec4 vColour;
in vec2 vUV;

out vec4 fragColor;

uniform vec4 cameraPosition;
uniform vec4 lightDirection;

uniform sampler2D DiffuseTexture;
 
 struct Light{
 vec4 ambient;
 vec4 diffuse;
 vec4 specular;
 };

 const Light sceneLight = Light(vec4( 0.25, 0.25,0.25,1.0), vec4(1.f,1.f,1.f,1.f), vec4(1.f,0.0f,0.0f,1.f));
 const float specularTerm = 32.f;

 
 uniform vec4  modelAmbient;
 uniform vec4  modelDiffuse;
 uniform vec4  modelSpecular;


 float orenNayarDiffuse(  vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, float roughness, float albedo)
{
  float LdotV = dot(lightDirection, viewDirection);
  float NdotL = dot(lightDirection, surfaceNormal);
  float NdotV = dot(surfaceNormal, viewDirection);

  float s = LdotV - NdotL * NdotV;
  float t = mix(1.0, max(NdotL, NdotV), step(0.0, s));

  float sigma2 = roughness * roughness;
  float A = 1.0 + sigma2 * (albedo / (sigma2 + 0.13) + 0.5 / (sigma2 + 0.33));
  float B = 0.45 * sigma2 / (sigma2 + 0.09);

  return albedo * max(0.0, NdotL) * (A + B * s / t) / PI;
}

void main()
{

	vec4 albedo = texture(DiffuseTexture, vUV);
	vec4 eyeDir = normalize( cameraPosition - vPos);
	float intensity = orenNayarDiffuse( -lightDirection.xyz, eyeDir.xyz, vNormal.xyz, 0.1, 0.9);
	fragColor = vec4(albedo.xyz * (intensity * modelDiffuse.xyz), 1.0f);
}


