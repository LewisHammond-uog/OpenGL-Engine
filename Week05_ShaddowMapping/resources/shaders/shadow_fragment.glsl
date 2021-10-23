#version 150

out float depth;

void main()
{
	depth = gl_FragColor.z;
}
