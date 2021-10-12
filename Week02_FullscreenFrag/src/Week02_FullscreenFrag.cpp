#include "Week02_FullscreenFrag.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>
#include <imgui.h>

#include "Application_Log.h"

Week02_FullscreenFrag::Week02_FullscreenFrag()
{

}

Week02_FullscreenFrag::~Week02_FullscreenFrag()
{

}

bool Week02_FullscreenFrag::onCreate()
{
	//Load in shaders
	m_vertexShaderID = Utility::loadShader("../shaders/vertex.glsl", GL_VERTEX_SHADER);
	m_fragmentShaderID = Utility::loadShader("../shaders/fragment.glsl", GL_FRAGMENT_SHADER);

	//Define shader pipeline inputs
	const char* szInputs[] = { "Position", "Colour" };
	//Define shader pipeline outputs
	const char* szOutputs[] = { "fragColour" };
	//link shaders together
	m_shaderProgramID = Utility::createProgram(m_vertexShaderID, 0, 0, 0, m_fragmentShaderID, 2, szInputs, 1, szOutputs);

	//Create some vertex data, one vert in each corner of the screen
	Vertex* verts = new Vertex[4];
	verts[0].position = glm::vec4(-1.f,-1.f, 0.f, 1.f);
	verts[1].position = glm::vec4(-1.f,1.f, 0.f, 1.f);
	verts[2].position = glm::vec4(1.f,-1.f, 0.f, 1.f);
	verts[3].position = glm::vec4(1.f,1.f, 0.f, 1.f);

	verts[0].colour = glm::vec4(1.f, 0.f, 0.f, 1.f);
	verts[1].colour = glm::vec4(0.f, 1.f, 0.f, 1.f);
	verts[2].colour = glm::vec4(0.f, 0.f, 1.f, 1.f);
	verts[3].colour = glm::vec4(1.f, 1.f, 1.f, 1.f);

	unsigned int indicies[] = 
	{
		0,2,1, //Triangle 1
		3,1,2 //Triangle 2
	};

	//generate and bind vertex array object
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo); //Generate VBO array
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo); //bind it to the VAO
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), verts, GL_STATIC_DRAW); //Tell the buffer that its the size of 4 verts and won't change (GL_STATIC_DRAW)

	glEnableVertexAttribArray(0); //Enable Vertex attrib 0 (position)
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)0 + sizeof(glm::vec4));


	//generate a fill index buffer
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indicies, GL_STATIC_DRAW);

	//Unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Verts are bound of the GPU - delete them on the CPU side
	delete[] verts;
	//same for indicies
	//delete[] indicies;

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f,0.25f,0.25f,1.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	return true;
}

void Week02_FullscreenFrag::Update(float a_deltaTime)
{

	static bool show_demo_window = true;
	//ImGui::ShowDemoWindow(&show_demo_window);
	Application_Log* log = Application_Log::Get();
	if (log != nullptr && show_demo_window)
	{
		log->showLog(&show_demo_window);
	}
	//show application log window
	if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_L) == GLFW_PRESS) {
		show_demo_window = !show_demo_window;
	}
	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void Week02_FullscreenFrag::Draw()
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_shaderProgramID);
	glBindVertexArray(m_vao);

	//Get the unifom mem location
	int timeUniformLocation = glGetUniformLocation(m_shaderProgramID, "Time");
	glUniform1f(timeUniformLocation, Utility::getTotalTime());

	glDrawElements(GL_TRIANGLES, 6 /*6 verts*/, GL_UNSIGNED_INT, 0 /*start at 0 in the indicies buffer*/);

	glBindVertexArray(0);
	glUseProgram(0);
}

void Week02_FullscreenFrag::Destroy()
{
	//Delete shaders
	glDeleteShader(m_vertexShaderID);
	glDeleteShader(m_fragmentShaderID);

	//Delete program
	glDeleteProgram(m_shaderProgramID);

	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
	glDeleteVertexArrays(1, &m_vao);
}

