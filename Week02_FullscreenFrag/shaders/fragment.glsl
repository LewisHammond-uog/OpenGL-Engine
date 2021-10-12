#version 330

//inputs from the previous pipeline stage
in vec4 vColour;

//output from the fragment stage
out vec4 fragColour;

//Get a uniform from the CPU side - constant for a single run through the render pipeline
uniform float u_Time;
uniform vec2 u_Resolution;

const float PI = 3.14159265359;

float Circle(in vec2 a_uv, in float a_radius)
{
	const vec2 circleCenter = vec2(0.5f, 0.5f);
	vec2 pos = circleCenter - a_uv; //How far from the center of the circle are we?
	a_radius *= 0.75;
	return 1 - smoothstep(a_radius - (a_radius * 0.05), a_radius + (a_radius * 0.05), dot(pos, pos) * PI); //return how much within the center of the circle we are
}


void main(){

	vec2 uv = gl_FragCoord.xy / u_Resolution; //Get the current pixel coord that we are drawing and get that as a UV coordinate
	
	//Multiply the UV by 3 and get just the fractional component of it so we can have the 
	//9 circles
	uv *= 30.0;
	uv = fract(uv);

	float sins = sin(u_Time);

	//If sin is less than zero invert the colours
	vec4 tempColour = vColour;
	if(sins < 0){
		tempColour = vColour.gbra;
	}

	fragColour = mix(vec4(0,0,0,1), tempColour, Circle(uv, abs(sins) * 3));
}