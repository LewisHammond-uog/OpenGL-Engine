#include "GeometryProgram.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

bool GeometryProgram::Initialise()
{
	//Base Initalize
	if(!ShaderProgram::Initialise())
	{
		return false;
	}

	//Add Shaders to program
	if(!AddShader(GL_VERTEX_SHADER, "../shaders/geo_vertex.glsl"))
	{
		return false;
	}
	if (!AddShader(GL_GEOMETRY_SHADER, "../shaders/geo_geometry.glsl"))
	{
		return false;
	}
	if (!AddShader(GL_FRAGMENT_SHADER, "../shaders/geo_fragment.glsl"))
	{
		return false;
	}

	//Compile Shaders
	if(!CompileProgram())
	{
		return false;
	}

	//Get uniform locations
	m_projectionViewLocation = GetUniformLocation("ProjectionView");
	m_cameraWorldPosLocation = GetUniformLocation("CameraPosition");
	m_textureLocation = GetUniformLocation("TextureSampler");

}

void GeometryProgram::SetupBuffers(const int a_numberOfStars)
{
	m_particleCount = a_numberOfStars;
	m_vertices = new glm::vec4[m_particleCount];

	// Generate and bind our Vertex Array Object
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	//Generate our Vertex Buffer Object
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_particleCount * sizeof(glm::vec4), m_vertices, GL_DYNAMIC_DRAW);

	glBindAttribLocation(m_glShaderProgramID, 0, "particlePos");

	//Enable position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), 0);

	//generate and bind an index buffer
	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	//the thrid argument of glBufferData is the data that will fill this buffer here it is set to nullptr
	//so this buffer will just have empty data in it
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_particleCount * sizeof(unsigned int), nullptr, GL_STATIC_DRAW);

	glBindVertexArray(0);
	//unbind our current vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//unbind our current index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	//Set our vertex and Index buffers in here as they don't get altered for this programs run through
	//set the vertices for our billboard -- there is no reason this couldn't be in the init function 
	//as these vertice data do not change over time.
	for(int i = 0; i < m_particleCount; ++i)
	{
		m_vertices[i] = glm::vec4(0.f, i * 5, 0.f, 1.0f);
	}


	//bind our vertex buffer and fill it with our mertex data
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_particleCount * sizeof(glm::vec4), m_vertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//bind our index buffer and specify the indices order in the indices array		
	constexpr unsigned int indices[] = { 0, 1, 2, 3 };
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_particleCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

}

void GeometryProgram::DrawParticles() const
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//bind our vertex array object
	glBindVertexArray(m_VAO);

	//depending on the state call draw with glDrawElements to draw our buffer
	//glDrawElements uses the index array in our index buffer to draw the vertices in our vertex buffer
	glDrawElements(GL_POINTS, m_particleCount, GL_UNSIGNED_INT, 0);

	glDisable(GL_BLEND);
}

void GeometryProgram::SetProjectionView(glm::mat4 a_projectionView) const
{
	glUniformMatrix4fv(m_projectionViewLocation, 1, GL_FALSE, glm::value_ptr(a_projectionView));
}

void GeometryProgram::SetCameraWorldPos(glm::vec4 a_cameraPos) const
{
	glUniform3fv(m_cameraWorldPosLocation, 1, glm::value_ptr(a_cameraPos));
}
