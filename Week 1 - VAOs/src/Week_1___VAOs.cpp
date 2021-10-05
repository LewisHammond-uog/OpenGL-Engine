#include "Week_1___VAOs.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>
#include <imgui.h>

#include "Application_Log.h"

Week_1___VAOs::Week_1___VAOs()
{

}

Week_1___VAOs::~Week_1___VAOs()
{

}

bool Week_1___VAOs::onCreate()
{
	// initialise the Gizmos helper class
	Gizmos::create();

	//Set Max Verts
	m_maxVerts = 36;

	//Load Shaders
	m_vertexShaderID = Utility::loadShader("../../shaders/vertex.glsl", GL_VERTEX_SHADER);
	m_fragmentShaderID = Utility::loadShader("../../shaders/fragment.glsl", GL_FRAGMENT_SHADER);

	//Define inputs and outputs for shaders
	const char* szInputs[] = { "position", "colour" };
	const char* szOutputs[] = { "fragColour" };

	//Link shaders together to create a shader program
	m_programID = Utility::createProgram(m_vertexShaderID, 0, 0, 0, m_fragmentShaderID, 2, szInputs, 1, szOutputs);

	//Create Verticies for Cube
	m_vertices = new Vertex[8];

	//Make a 2x2 unit cube
	glm::vec4 vX(2.f, 0.f, 0.f, 0.f);
	glm::vec4 vY(0.f, 2.f, 0.f, 0.f);
	glm::vec4 vZ(0.f, 0.f, 2.f, 0.f);

	glm::vec4 cubeOrigin(0.f, 0.f, 0.f, 1.f);

	//back part of cube
	m_vertices[0].position = cubeOrigin - vX - vY - vZ;
	m_vertices[1].position = cubeOrigin - vX + vY - vZ;
	m_vertices[2].position = cubeOrigin + vX + vY - vZ;
	m_vertices[3].position = cubeOrigin + vX - vY - vZ;
	//front part of cube
	m_vertices[4].position = cubeOrigin - vX - vY + vZ;
	m_vertices[5].position = cubeOrigin - vX + vY + vZ;
	m_vertices[6].position = cubeOrigin + vX + vY + vZ;
	m_vertices[7].position = cubeOrigin + vX - vY + vZ;

	for(int i = 0; i < 8; ++i)
	{
		m_vertices[i].colour = glm::vec4((float)(i % 3 / 3.f), (float)(i % 4 / 4.f), (float)(i % 7 / 7.f), 1.f);
	}

	//Order to link the indecies to draw triangles
	unsigned int indicies[] = {
		2, 1, 0,
		3, 2, 0,
		5, 6, 4,
		6, 7, 4,
		4, 3, 0,
		7, 3, 4,
		1, 2, 5,
		2, 6, 5,
		0, 1, 4,
		1, 5, 4,
		2, 3, 7,
		6, 2, 7
	};

	//Create Vertex Array Object
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	//Create and bind the Vertex Buffer
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(Vertex), m_vertices, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0); //Position
	glEnableVertexAttribArray(1); //Colour
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0); //Attribute 0 has 4 floats as members that are not normalized, stide the size of vertex to find the next one, start at pos 0 - POSITION
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char*)0 + sizeof(glm::vec4)) ); //Attribute 1 has 4 floats as members that are not normalized, stide the size of vertex to find the next one, start at pos 16 (0 + the size of a vec4) - COLOUR

	//Create a bind the index buffer - do the same as above but for indecies
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_maxVerts * sizeof(unsigned int), indicies, GL_STATIC_DRAW);

	//This is now unbinding everything - 0 is a null buffer
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	
	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, m_windowWidth/(float)m_windowHeight, 0.1f, 1000.0f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f,0.25f,0.25f,1.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	return true;
}

void Week_1___VAOs::Update(float a_deltaTime)
{
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	// clear all gizmos from last frame
	Gizmos::clear();
	
	// add an identity matrix gizmo
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	// add a 20x20 grid on the XZ-plane
	for ( int i = 0 ; i < 21 ; ++i )
	{
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10), 
						 i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i), 
						 i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	
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

void Week_1___VAOs::Draw()
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	// draw the gizmos from this frame
	Gizmos::draw(viewMatrix, m_projectionMatrix);

	//Use our shader to draw
	glUseProgram(m_programID);
	//bind our vertex array to get the buffers we bound onCreate
	glBindVertexArray(m_vao);
	//Set shader uniform data - from CPU to GPU
	int projectionViewUniformLocation = glGetUniformLocation(m_programID, "ProjectionView");

	Application_Log* log = Application_Log::Create();
	if(log && projectionViewUniformLocation < 0)
	{
		log->addLog(LOG_WARNING, "projectionViewUniformLocation was not valid %i", projectionViewUniformLocation);
	}

	glUniformMatrix4fv(projectionViewUniformLocation, 1, GL_FALSE, glm::value_ptr(m_projectionMatrix * viewMatrix));
	glCullFace(GL_FRONT);
	glDrawElements(GL_TRIANGLES, m_maxVerts, GL_UNSIGNED_INT, 0);

	//Unbind
	glBindVertexArray(0);
	glUseProgram(0);

}

void Week_1___VAOs::Destroy()
{
	Gizmos::destroy();

	glDeleteBuffers(1, &m_ibo);
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);

	glDeleteProgram(m_programID);
	glDeleteShader(m_vertexShaderID);
	glDeleteShader(m_fragmentShaderID);
}

