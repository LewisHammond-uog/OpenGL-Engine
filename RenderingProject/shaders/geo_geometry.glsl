#version 330

layout(points) in;

//We are going to output a triangle strip with 4 verts
layout(triangle_strip, max_vertices = 4) out;

out vec4 vColour;
out vec2 vUV;

uniform mat4 ProjectionView;
uniform vec3 CameraPosition;

const float ParticleSize = 2.0f;

void main(){

	//Get the position of the vert from the output of the vertex shader (gl_position)
	vec3 particlePos = gl_in[0].gl_Position.xyz;

	//Create a billboard - so that the particle is facing the camera
	mat3 billboard;
	billboard[2] = normalize( CameraPosition.xyz - particlePos);
	billboard[0] = cross( vec3(0,1,0), billboard[2] );
	billboard[1] = cross( billboard[2], billboard[0]);

	//Calculate half of the particle size
	float halfSize = ParticleSize * 0.5f;

	//Create the 4 verts for the corners of the billboard
	const int vertCount = 4;
	vec3 verts[vertCount];
	verts[0] = particlePos + billboard * vec3(-halfSize, -halfSize, 0);
	verts[1] = particlePos + billboard * vec3(halfSize, -halfSize, 0);
	verts[2] = particlePos + billboard * vec3(-halfSize, halfSize, 0);
	verts[3] = particlePos + billboard * vec3(halfSize, halfSize, 0);

	//set the uv coordiantes for the verts being emitted
	vec2 UVCoords [4];
	UVCoords[0] = vec2( 0.f, 0.f);
	UVCoords[1] = vec2( 1.f, 0.f);
	UVCoords[2] = vec2( 0.f, 1.f);
	UVCoords[3] = vec2( 1.f, 1.f);


	//Loop and emit the verts for this primative
	for(int i = 0; i < vertCount; ++i){
		vColour = vec4(1,1,1,1);
		vUV = UVCoords[i];
		gl_Position = ProjectionView * vec4(verts[i], 1);

		//Emmit Vertex to tell geometry shader that we have created the vert
		EmitVertex();
	}
	EndPrimitive();
}