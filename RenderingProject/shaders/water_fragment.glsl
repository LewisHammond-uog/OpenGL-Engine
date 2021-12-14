#version 330
in vec3 finalColour;
out vec4 fragColour;

void main(){
	fragColour = vec4(finalColour, 0.5);
}