#version 330

//input params - things that come into the shader from the vertex buffer
in vec4 Position;
in vec4 Colour;
in vec2 TexCoord;

//output from the vertex stage of the pipeline
out vec4 vColour;
out vec2 vTexCoord;

void main(){
	vColour = Colour; //pass the colour to the next stage of the pipeline
	vTexCoord = TexCoord;
	gl_Position = Position; //output vertex to openGL - Position is already in screen space position
}