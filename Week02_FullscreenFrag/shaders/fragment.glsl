#version 330

//inputs from the previous pipeline stage
in vec4 vColour;

//output from the fragment stage
out vec4 fragColour;

//Get a uniform from the GPU side - constant for a single run through the render pipeline
uniform float Time;

void main(){
	fragColour = abs(sin(Time)) * vColour;
}