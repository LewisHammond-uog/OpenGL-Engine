#version 150

in vec4 vPos;
in vec4 vNormal;
in vec4 vColour; 
in vec2 vUV;
in vec4 vLightDir;
in vec4 vViewDir;

out vec4 fragColour;

uniform vec4 cameraPosition;
uniform vec4 lightDirection;

uniform vec4 lightColour = vec4(1.0, 0.85, 0.05, 1.0);
uniform vec4 lightAmbient = vec4(0.4, 0.4, 0.4, 1.0);
uniform vec4 lightSpecular = vec4(1.0, 1.0, 1.0, 1.0);

uniform float specularPower = 25.0;

uniform sampler2D DiffuseTexture;
uniform sampler2D NormalTexture;

uniform int useNormalMap = 1;
 
void main()
{
	vec4 albedo = texture(DiffuseTexture, vUV);
	vec4 normalTex;

	if(useNormalMap == 1){
		//Sample the Normal texture and put it in to -1 to 1 rather than 0 to 1
		normalTex = (texture(NormalTexture, vUV) * 2.0) - 1.0;
	}else{
		normalTex = vNormal;
	}

	vec4 ambient = lightAmbient;

	float NdL = max(0.0, dot(vNormal, -vLightDir)); //get the lambertian term
	vec4 diffuse = lightColour * NdL; //Use the light colour as the diffuse colour of the light

	vec4 R = reflect(vLightDir, normalTex); //use the built in reflect function to reflect the light direction around the surface normal
	float specularTerm = max(0.f, pow(dot(R, vViewDir), specularPower));
	vec4 specular = lightSpecular * specularTerm;

	fragColour = vec4(albedo.rgb * (ambient.rgb + diffuse.rgb + specular.rgb), 1.0);
}
