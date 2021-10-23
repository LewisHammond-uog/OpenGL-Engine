#include "Reflection.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>
#include "Application_Log.h"
#include <SVertex.h>

Reflection::Reflection()
{

}

Reflection::~Reflection()
{

}

bool Reflection::onCreate()
{
	// initialise the Gizmos helper class
	Gizmos::create();

	//------Load the standard shader that will render to the frame buffer----------

	//Load the shaders for this program
	unsigned int vertexShader = Utility::loadShader("../resources/shaders/vertex.glsl", GL_VERTEX_SHADER);
	unsigned int fragmentShader = Utility::loadShader("../resources/shaders/fragment.glsl", GL_FRAGMENT_SHADER);
	//Define the input and output varialbes in the shaders
	//Note: these names are taken from the glsl files -- added in inputs for UV coordinates
	const char* szInputs[] = { "Position", "Colour", "Normal","Tex1" };
	const char* szOutputs[] = { "FragColor" };
	//bind the shaders to create our shader program
	m_fboprogramID = Utility::createProgram(
		vertexShader,
		0,
		0,
		0,
		fragmentShader,
		4, szInputs, 1, szOutputs);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//------Load the full screen quad render shader----------
	vertexShader = Utility::loadShader("../resources/shaders/fs_vertex.glsl", GL_VERTEX_SHADER);
	fragmentShader = Utility::loadShader("../resources/shaders/fs_fragment.glsl", GL_FRAGMENT_SHADER);

	const char* szInputs2[] = { "Position", "Tex1" };
	const char* szOutputs2[] = { "FragColour" };

	//bind shaders to create program
	m_fsprogramID = Utility::createProgram(
		vertexShader,
		0,
		0,
		0,
		fragmentShader,
		2, szInputs2, 1, szOutputs2);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//--------------------

	m_fbxModel = new FBXFile();
	m_fbxModel->load("../resources/models/stanford/Dragon.fbx", FBXFile::UNITS_DECIMETER);

	//----Setup Vertex Attributes for 1st pass render --
	//Generate our OpenGL Vertex and Index Buffers for rendering our FBX Model Data
	// OPENGL: genorate the VBO, IBO and VAO
	glGenBuffers(2, m_vbo);
	glGenBuffers(2, m_ibo);
	glGenVertexArrays(1, m_vao);

	// OPENGL: Bind  VAO, and then bind the VBO and IBO to the VAO
	glBindVertexArray(m_vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo[0]);

	//There is no need to populate the vbo & ibo buffers with any data at this stage
	//this can be done when rendering each mesh component of the FBX model

	// enable the attribute locations that will be used on our shaders
	glEnableVertexAttribArray(0); //Pos
	glEnableVertexAttribArray(1); //Col
	glEnableVertexAttribArray(2); //Norm
	glEnableVertexAttribArray(3); //Tex1
								  // tell our shaders where the information within our buffers lie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char *)0) + FBXVertex::PositionOffset);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char *)0) + FBXVertex::ColourOffset);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char *)0) + FBXVertex::NormalOffset);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char *)0) + FBXVertex::TexCoord1Offset);

	// finally, where done describing our mesh to the shader
	// we can describe the next mesh
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//------------------------------------------
	//---------Setup for 2nd pass render ------------
	//Create a plane to put the reflection on
	SVertex vertexData[4];
	vertexData[0] = SVertex(glm::vec4(-10.f, 0.f, -10.f, 1.f), glm::vec2(0,1));
	vertexData[1] = SVertex(glm::vec4(-10.f, 0.f, 10.f, 1.f), glm::vec2(0, 0));
	vertexData[2] = SVertex(glm::vec4(10.f, 0.f, -10.f, 1.f), glm::vec2(1, 1));
	vertexData[3] = SVertex(glm::vec4(10.f, 0.f, 10.f, 1.f), glm::vec2(1, 0));

	//Set triangle draw order
	unsigned int elements[6] = { 0,1,2,1,3,2 };

	//Generate Open GL Vertex and Index Buffers
	glBindVertexArray(m_vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(SVertex), vertexData, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), elements, GL_STATIC_DRAW);

	//Enable attribute locations
	glEnableVertexAttribArray(0); //pos
	glEnableVertexAttribArray(1); //tex1

	//tell shaders where the infomation in the buffers is
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(SVertex), ((char*)0) + SVertex::PositionOffset);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(SVertex), ((char*)0) + SVertex::TexCoord1Offset);

	//Unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	//-----Frame Buffer Object-------//

	//Generate a frame buffer
	glGenFramebuffers(1, &m_FBO);

	//Bind the frame buffer for editing
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	//Create a texture to be attached to the frame buffer
	glGenTextures(1, &m_FBO_texture);

	//Bind Texture for editing
	glBindTexture(GL_TEXTURE_2D, m_FBO_texture);

	//Create the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_windowWidth, m_windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	//Set filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//---- Generate the depth texture ----//
	glGenTextures(1, &m_FBO_depth_texture);
	glBindTexture(GL_TEXTURE_2D, m_FBO_depth_texture);//set as active texture 2d

	//Generate texture for the depth texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, m_windowWidth, m_windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	///Attach the texture as the 0th colour attachment of the frame buffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_FBO_texture, 0);
	//Attach the depth texturE
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_FBO_depth_texture, 0);

	//Set which frame buffers to draw to
	constexpr GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	//Check for error in creation
	GLenum frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (frameBufferStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		Application_Log* log = Application_Log::Get();
		if (log != nullptr)
		{
			log->addLog(LOG_ERROR, "Error in creating framebuffer");
		}
	}


	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse(glm::lookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));

	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, (float)m_windowWidth / (float)m_windowHeight, 0.1f, 1000.0f);

	m_modelMatrix = glm::mat4();

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f, 0.25f, 0.25f, 1.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	m_lightPos = glm::vec4(50.f, 25.f, 0.f, 1.f);
	return true;
}

void Reflection::Update(float a_deltaTime)
{
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement(m_cameraMatrix, a_deltaTime, 10);

	// clear all gizmos from last frame
	Gizmos::clear();

	// add an identity matrix gizmo
	Gizmos::addTransform(glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));

	// rotate point
	float s = sinf(a_deltaTime * 0.2f);
	float c = cosf(a_deltaTime * 0.2f);
	glm::vec4 temp = m_lightPos;
	m_lightPos.x = temp.x * c - temp.z * s;
	m_lightPos.z = temp.x * s + temp.z * c;
	m_lightPos.w = 0;
	m_lightPos = glm::normalize(m_lightPos) * 25.f;
	Gizmos::addBox(m_lightPos.xyz, glm::vec3(0.2f, 0.2f, 0.2f), true, glm::vec4(1.0f, 0.85f, 0.05f, 1.f));


	//Update our FBX model
	for (unsigned int i = 0; i < m_fbxModel->getMeshCount(); ++i)
	{
		// get the current mesh
		FBXMeshNode *mesh = m_fbxModel->getMeshByIndex(i);
		// if you move an object around within your scene
		// children nodes are not updated until this function is called.
		mesh->updateGlobalTransform();
	}

	//ImGui render what is in the FBO texture
	ImGui::SetNextWindowPos(ImVec2(m_windowWidth - m_windowWidth * 0.3, m_windowHeight - m_windowHeight * 0.4));
	ImGui::SetNextWindowSize(ImVec2(m_windowWidth * 0.3, m_windowHeight * 0.4));
	ImGui::Begin("Frame Buffer");
	ImGui::BeginTabBar("FrameBuffer Textures");

	ImTextureID textureID = nullptr;
	if(ImGui::BeginTabItem("Colour Buffer"))
	{
		textureID = (void*)(intptr_t)m_FBO_texture;
		ImGui::EndTabItem();
	}

	if(ImGui::BeginTabItem("Depth Buffer"))
	{
		textureID = (void*)(intptr_t)m_FBO_depth_texture;
		ImGui::EndTabItem();
	}

	if (textureID) {
		ImGui::Image(textureID, ImVec2(m_windowWidth * 0.25, m_windowHeight * 0.25), ImVec2(0, 1), ImVec2(1, 0));
	}
	ImGui::EndTabBar();
	ImGui::End();



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
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void Reflection::Draw()
{
	//Bind the frame buffer for rendering
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	// clear the backbuffer
	glClearColor(0.25f, 0.25f, 0.25f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse(m_cameraMatrix);

#pragma region First Pass Render
	//bing our shader program
	glUseProgram(m_fboprogramID);
	//bind our vertex array object
	glBindVertexArray(m_vao[0]);

	//get our shaders uniform location for our projectionViewMatrix and then use glUniformMatrix4fv to fill it with the correct data
	unsigned int projectionViewUniform = glGetUniformLocation(m_fboprogramID, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(m_projectionMatrix * viewMatrix));

	//pass throught the view matrix
	unsigned int viewMatrixUniform = glGetUniformLocation(m_fboprogramID, "ViewMatrix");
	glUniformMatrix4fv(viewMatrixUniform, 1, false, glm::value_ptr(viewMatrix));

	//pass our camera position to our fragment shader uniform
	unsigned int cameraPosUniform = glGetUniformLocation(m_fboprogramID, "cameraPosition");
	glUniform4fv(cameraPosUniform, 1, glm::value_ptr(m_cameraMatrix[3]));


	//pass the directional light direction to our fragment shader
	glm::vec4 lightDir = -m_lightPos;
	lightDir.w = 0.f;
	lightDir = glm::normalize(lightDir);
	unsigned int lightDirUniform = glGetUniformLocation(m_fboprogramID, "lightDirection");
	glUniform4fv(lightDirUniform, 1, glm::value_ptr(lightDir));

	//Draw our FBX Model

	// for each mesh in the model...
	for (unsigned int i = 0; i < m_fbxModel->getMeshCount(); ++i)
	{
		// get the current mesh
		FBXMeshNode *pMesh = m_fbxModel->getMeshByIndex(i);

		// send the Model
		glm::mat4 m4Model = pMesh->m_globalTransform;// *m_modelMatrix;
		unsigned int modelUniform = glGetUniformLocation(m_fboprogramID, "Model");
		//glUniformMatrix4fv(modelUniform, 1, false, glm::value_ptr(m4Model));
		glm::mat4 reflectionMatrix = glm::mat4(1.0);
		reflectionMatrix = glm::reflect3D(reflectionMatrix, glm::vec3(0, 1, 0));
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(reflectionMatrix * m4Model));

		//send the normal matrix
		//this is only beneficial if a model has a non-uniform scale or non-orthoganal model matrix
		glm::mat4 m4Normal = glm::transpose(glm::inverse(pMesh->m_globalTransform));// *m_modelMatrix;
		unsigned int normalMatrixUniform = glGetUniformLocation(m_fboprogramID, "NormalMatrix");
		glUniformMatrix4fv(normalMatrixUniform, 1, false, glm::value_ptr(m4Normal));

		// Send the vertex data to the VBO
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, pMesh->m_vertices.size() * sizeof(FBXVertex), pMesh->m_vertices.data(), GL_STATIC_DRAW);

		// send the index data to the IBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, pMesh->m_indices.size() * sizeof(unsigned int), pMesh->m_indices.data(), GL_STATIC_DRAW);

		//Because we have reflected geometry we need to flip the draw order of triangles
		glFrontFace(GL_CW);
		glDrawElements(GL_TRIANGLES, pMesh->m_indices.size(), GL_UNSIGNED_INT, 0);
		glFrontFace(GL_CCW);
	}
#pragma endregion
#pragma region 2nd Pass Rendering  whats in the frame buffer
	//stop rendering to the frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Clear
	glClearColor(0.0f, 0.0f, 0.0f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//use the fullscreen program 
	glUseProgram(m_fsprogramID);
	glBindVertexArray(m_vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo[1]);

	//Send the screenszie
	unsigned int screenSizeUniform = glGetUniformLocation(m_fsprogramID, "ScreenSize");
	glm::vec2 screenSize = glm::vec2(m_windowWidth, m_windowHeight);
	glUniform2fv(screenSizeUniform, 1, glm::value_ptr(screenSize));

	//Send out projection view matrix
	projectionViewUniform = glGetUniformLocation(m_fsprogramID, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, GL_FALSE, glm::value_ptr(m_projectionMatrix * viewMatrix));

	//Send model matrix
	glm::mat4 m4Model = glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	unsigned int modelUniform = glGetUniformLocation(m_fsprogramID, "Model");
	glUniformMatrix4fv(modelUniform, 1, false, glm::value_ptr(m4Model));


	//Set diffuse texture to be texture 0
	//unsigned int texUniformLocation = glGetUniformLocation(m_fsprogramID, "DiffuseTexture");
	//glUniform1i(texUniformLocation, 0);

	//Set our active texture and give it the texture that the frame buffer output to
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_FBO_texture);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
#pragma endregion
}

void Reflection::Destroy()
{

	m_fbxModel->unload();
	delete m_fbxModel;

	glDeleteBuffers(2, m_vbo);
	glDeleteBuffers(2, m_ibo);
	glDeleteVertexArrays(2, m_vao);
	glDeleteProgram(m_fsprogramID);
	glDeleteProgram(m_fboprogramID);

	Gizmos::destroy();
}

