#version 330

in float particleType;
in vec4 particlePos;
in vec4 particleVelocity;
in float particleAge;

out vec4 vColour;

void main(){
	vColour = vec4(1,1,1,1);
	gl_Position = particlePos;
}