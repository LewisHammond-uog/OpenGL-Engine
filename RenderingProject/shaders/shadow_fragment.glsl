#version 330

out float depth;

void main(){
	depth = gl_FragColor.z;
}