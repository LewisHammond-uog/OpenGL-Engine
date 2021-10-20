#version 330

in vec4 vPosition;
in vec4 vColour;
in vec4 vNormal;
in vec2 vTexCoord;

out vec4 fragColour;

uniform vec4 CameraPosition;
uniform vec4 LightDirection;
uniform vec4 LightColour;

uniform vec4 matAmbient;
uniform vec4 matDiffuse;
uniform vec4 matSpecular;

uniform sampler2D DiffuseTexture;

//Ambient light colour
const vec4 ambientLight = vec4(0.25, 0.25, 0.25, 1.f);

//Ambient Light specular colour
const vec4 LightSpecular = vec4(1.0, 1.0, 1.0, 1.0);
const float specularPower = 125.f;

void main(){
	vec4 albedo = texture(DiffuseTexture, vTexCoord);

	//Directional Light
	//diffuse * diffuse light colour * n.l
	vec4 ambient = ambientLight * matAmbient;
	vec4 diffuse = matDiffuse * LightColour * max(dot(vNormal, -LightDirection), 0.f);

	vec4 eyeDirection = normalize(CameraPosition - vPosition);
	vec4 reflectionVector = reflect(LightDirection, vNormal);
	vec4 specular = matSpecular * LightSpecular * max(0.f, pow(dot(eyeDirection, reflectionVector), specularPower));

	fragColour = albedo * (ambientLight + diffuse + specular);
}