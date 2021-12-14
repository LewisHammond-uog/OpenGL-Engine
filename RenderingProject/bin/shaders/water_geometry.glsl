#version 150

layout ( triangles ) in;
layout ( triangle_strip, max_vertices = 3) out;

out vec3 finalColour;

uniform mat4 uWorldViewPoint;
uniform vec3 cameraPosition;
uniform vec3 ulightDirection;
uniform vec3 ulightColour;

const vec3 waterColour = vec3(0.1, 0.2, 0.25);
const float reflectivity = 0.002f;
const float shineDamper = 1.0;
const float ambientLighting = 0.0;

vec3 calculateTriangleNormal(){
	vec3 tangent = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 bitangent = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 normal = cross(tangent, bitangent);	
	return normalize(normal);
}

vec3 calculateSpecular(vec4 worldPosition, vec3 normal){
	vec3 viewVector = normalize(cameraPosition - worldPosition.xyz);
	vec3 reflectedLightDirection = reflect(ulightDirection, normal);
	float specularFactor = dot(reflectedLightDirection, viewVector);
	specularFactor = max(pow(specularFactor, shineDamper), 0.0);
	return ulightColour * specularFactor * reflectivity;
}

void main(void){

	vec3 normal = calculateTriangleNormal();
	float brightness = max(dot(-ulightDirection, normal), ambientLighting) * 0.2f;
	vec3 colour = waterColour * brightness;

	vec4 worldPosition = gl_in[0].gl_Position;
	gl_Position = uWorldViewPoint * worldPosition;
	finalColour = colour + calculateSpecular(worldPosition, normal);
	EmitVertex();
	
	worldPosition = gl_in[1].gl_Position;
	gl_Position = uWorldViewPoint * worldPosition;
	finalColour = colour+ calculateSpecular(worldPosition, normal);
	EmitVertex();
	
	worldPosition = gl_in[2].gl_Position;
	gl_Position = uWorldViewPoint * worldPosition;
	finalColour = colour+ calculateSpecular(worldPosition, normal);
	EmitVertex();
	
	EndPrimitive();

}