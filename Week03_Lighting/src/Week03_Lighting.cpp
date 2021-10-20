#include "Week03_Lighting.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>
#include <imgui.h>

#include "Application_Log.h"

Week03_Lighting::Week03_Lighting()
{

}

Week03_Lighting::~Week03_Lighting()
{

}

bool Week03_Lighting::onCreate()
{
	// initialise the Gizmos helper class
	Gizmos::create();
	
	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	
	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, m_windowWidth/(float)m_windowHeight, 0.1f, 1000.0f);


	//Load in FBX Model
	m_fbxModel = new FBXFile();
	bool didLoad = m_fbxModel->load("../models/ruinedtank/tank.fbx", FBXFile::UNITS_CENTIMETER);

	//Load in and link shaders
	m_vertexShaderID = Utility::loadShader("../shaders/vertex.glsl", GL_VERTEX_SHADER);
	m_fragmentShaderID = Utility::loadShader("../shaders/fragment.glsl", GL_FRAGMENT_SHADER);
	//Vertex Inputs/Outputs
	const char* szInputs[] = { "Position", "Colour", "Normal", "TexCoord" };
	const char* szOutputs[] = { "fragColour" };

	//Link shaders
	m_programID = Utility::createProgram(m_vertexShaderID, 0, 0, 0, m_fragmentShaderID, 4, szInputs, 1, szOutputs);

	//Generate Vertex Array Object
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	
	//Create 2 buffers for the model buffer array, 1 is the Vertex Buffer one is the Index Buffer
	glGenBuffers(2, m_modelBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_modelBuffer[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_modelBuffer[1]);

	//Enable attributes for all of the inputs
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::PositionOffset); //Set pointer to position
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::ColourOffset); //... colour 
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset); //... normals 
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset); //... texcoord

	//Reset
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Model position
	m_modelMatrix = glm::mat4();

	//Light Postion
	m_lightPos = glm::vec4(20.f, 15.f, 0.f, 1.f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f, 0.25f, 0.25f, 1.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	return true;
}

void Week03_Lighting::Update(float a_deltaTime)
{
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	// clear all gizmos from last frame
	Gizmos::clear();
	
	//Move light around the scene in a clockwise rotation
	float sine = sinf(a_deltaTime);
	float cos = cosf(a_deltaTime);
	glm::vec4 newLightPos = m_lightPos;
	m_lightPos.x = newLightPos.x * cos - newLightPos.z * sine;
	m_lightPos.z = newLightPos.x * sine + newLightPos.z * cos;

	//Add box at the light position
	Gizmos::addBox(m_lightPos, glm::vec3(0.2f, 0.2f, 0.2f), true, glm::vec4(1.f, 0.82f, 0.5f, 1.f));

	//For each mesh in the model file update children
	for (int i = 0; i < m_fbxModel->getMeshCount(); ++i) 
	{
		FBXMeshNode* mesh = m_fbxModel->getMeshByIndex(i);
		mesh->updateGlobalTransform();
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

void Week03_Lighting::Draw()
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	// draw the gizmos from this frame
	Gizmos::draw(viewMatrix, m_projectionMatrix);
	
	//Draw model using our shader
	glUseProgram(m_programID);
	glBindVertexArray(m_vao);

	//-- Pass through our uniforms --
	
	int projectionViewUniformLocation = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniformLocation, 1, false, glm::value_ptr(m_projectionMatrix * viewMatrix));

	int cameraPositionUniformLocation = glGetUniformLocation(m_programID, "CameraPosition");
	glUniform4fv(cameraPositionUniformLocation, 1, glm::value_ptr(m_cameraMatrix[3]));

	glm::vec4 lightDir = glm::normalize(glm::vec4(0.f, 0.f, 0.f, 1.f) - m_lightPos);
	int lightDirectionUniformLocation = glGetUniformLocation(m_programID, "LightDirection");
	glUniform4fv(lightDirectionUniformLocation, 1, glm::value_ptr(lightDir));

	int lightColourUniformLocation = glGetUniformLocation(m_programID, "LightColour");
	glUniform4fv(lightColourUniformLocation, 1, glm::value_ptr(glm::vec4(1.f, 0.85f, 0.05f, 1.f)));

	for (int i = 0; i < m_fbxModel->getMeshCount(); ++i) 
	{
		FBXMeshNode* mesh = m_fbxModel->getMeshByIndex(i);

		//set each model matrix for mesh
		int modelMatrixUniformLocation = glGetUniformLocation(m_programID, "ModelMatrix");
		glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(mesh->m_globalTransform));

		//Send the normals matrix
		//int normalMatrixUniformLocation = glGetUniformLocation(m_programID, "NormalMatrix");
		//glUniformMatrix4fv(normalMatrixUniformLocation, 1, false, glm::value_ptr(glm::transpose(glm::inverse(mesh->m_globalTransform))));

		//Send the ambient, diffuse and specular materials
		//int materialAmbientUniformLocation = glGetUniformLocation(m_programID, "matAmbient");
		//glUniform4fv(materialAmbientUniformLocation, 1, glm::value_ptr(mesh->m_material->ambient));

		//int materialDiffuseUniformLocation = glGetUniformLocation(m_programID, "matDiffuse");
		//glUniform4fv(materialDiffuseUniformLocation, 1, glm::value_ptr(mesh->m_material->diffuse));

		//int materialSpecularUniformLocation = glGetUniformLocation(m_programID, "matSpecular");
		//glUniform4fv(materialSpecularUniformLocation, 1, glm::value_ptr(mesh->m_material->specular));


		//textures on the model to send though
		int texUnifomLocation = glGetUniformLocation(m_programID, "DiffuseTexture");
		glUniform1i(texUnifomLocation, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->m_material->textureIDs[FBXMaterial::DiffuseTexture]);
		
		//Send through the vets
		glBindBuffer(GL_ARRAY_BUFFER, m_modelBuffer[0]);
		glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(FBXVertex), mesh->m_vertices.data(), GL_STATIC_DRAW);

		//Send through indecies
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_modelBuffer[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(unsigned int), mesh->m_indices.data(), GL_STATIC_DRAW);

		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}

	//Reset
	glBindVertexArray(0);
	glUseProgram(0);

}

void Week03_Lighting::Destroy()
{
	delete m_fbxModel;
	Gizmos::destroy();
}

