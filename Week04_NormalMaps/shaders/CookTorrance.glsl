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

 const Light sceneLight = Light(vec4( 0.25, 0.25,0.25,1.0), vec4(1.f,1.f,1.f,1.f), vec4(0.4f,0.40f,0.40f,1.f));
 const float specularTerm = 32.f;

 
 uniform vec4  modelAmbient;
 uniform vec4  modelDiffuse;
 uniform vec4  modelSpecular;

 uniform vec3 RAF;

 //Calculate diffuse lighting using Oren Nayar Caclulation
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

//beckamn distribution formula modified from Wikipedia - https://en.wikipedia.org/wiki/Specular_highlight
//this is for surfaces that have microfacets used in PBR
 float beckmannDistribution(float NdotH, float roughness) 
 {
	NdotH = max(NdotH, 0.0001);
	float cos2Alpha = NdotH * NdotH;
	float tan2Alpha = (cos2Alpha - 1.0) / cos2Alpha;
	float roughness2 = roughness * roughness;
	float denom = 3.141592653589793 * roughness2 * cos2Alpha * cos2Alpha;
	return exp(tan2Alpha / roughness2) / denom;
}

//Cook torrance specular highlight function adapted from wikipedia formula - https://en.wikipedia.org/wiki/Specular_highlight
float CookTorrance(  vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, float roughness, float fresnel)
{
	float VdotN = max(dot(viewDirection, surfaceNormal), 0.0);
	float LdotN = max(dot(lightDirection, surfaceNormal), 0.0);

	//Half angle vector
	vec3 H = normalize(lightDirection + viewDirection);

	//Geometric term
	float NdotH = max(dot(surfaceNormal, H), 0.0);
	float VdotH = max(dot(viewDirection, H), 0.000001);
	float x = 2.0 * NdotH / VdotH;
	float G = min(1.0, min(x * VdotN, x * LdotN));
  
	//Distribution term
	float D = beckmannDistribution(NdotH, roughness);

	//Fresnel term
	float F = pow(1.0 - VdotN, fresnel);

	//Multiply terms and done
	return max(0,G * F * D / max(3.14159265 * VdotN * LdotN, 0.000001));
}

void main()
{

	vec4 albedo = texture(DiffuseTexture, vUV);
	vec4 eyeDir = normalize( cameraPosition - vPos);
	float diffuse = orenNayarDiffuse( -lightDirection.xyz, eyeDir.xyz, vNormal.xyz, RAF.x, RAF.y);
	float specular = CookTorrance( -lightDirection.xyz, eyeDir.xyz, vNormal.xyz, RAF.x, RAF.z);
	
	vec3 lightResolution = vec3(diffuse) * sceneLight.diffuse.xyz + (sceneLight.ambient.xyz* modelAmbient.xyz) + (vec3(specular)*sceneLight.specular.xyz);
	fragColor = vec4(albedo.xyz * lightResolution, 1.0f);
}


