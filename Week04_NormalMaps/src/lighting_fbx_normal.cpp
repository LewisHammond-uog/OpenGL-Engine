#include "lighting_fbx_normal.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>
#include "Application_Log.h"


#include <stb_image.h>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif //!STB_IMAGE_IMPLEMENTATION


#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720


Lighting_FBX_Normal::Lighting_FBX_Normal()
{

}

Lighting_FBX_Normal::~Lighting_FBX_Normal()
{

}

bool Lighting_FBX_Normal::onCreate()
{
	// initialise the Gizmos helper class
	Gizmos::create();
	
	//Load the shaders for this program
	m_vertexShader = Utility::loadShader("./shaders/vertex.glsl", GL_VERTEX_SHADER);
	m_fragmentShader = Utility::loadShader("./shaders/fragment.glsl", GL_FRAGMENT_SHADER);
	//Define the input and output varialbes in the shaders
	//Note: these names are taken from the glsl files -- added in inputs for UV coordinates
	const char* szInputs[] = { "Position", "Colour", "Normal", "Tangent", "BiTangent", "Tex1" };
	const char* szOutputs[] = { "fragColor" };
	//bind the shaders to create our shader program
	m_programID = Utility::createProgram(
		m_vertexShader,
		0,
		0,
		0,
		m_fragmentShader,
		6, szInputs, 1, szOutputs);


	m_fbxModel = new FBXFile();
	m_fbxModel->load("./models/ruinedtank/tank.fbx", FBXFile::UNITS_CENTIMETER);

	//Load normal maps - only needed because they are not bound to the tank properly
	LoadImageFromFile("./models/ruinedtank/left_engine_norm.png", m_normalTextures[0]);
	LoadImageFromFile("./models/ruinedtank/turret_norm.png", m_normalTextures[1]);

	//Generate our OpenGL Vertex and Index Buffers for rendering our FBX Model Data
	// OPENGL: genorate the VBO, IBO and VAO
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);
	glGenVertexArrays(1, &m_vao);

	// OPENGL: Bind  VAO, and then bind the VBO and IBO to the VAO
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

	//There is no need to populate the vbo & ibo buffers with any data at this stage
	//this can be done when rendering each mesh component of the FBX model

	// enable the attribute locations that will be used on our shaders
	glEnableVertexAttribArray(0); //Pos
	glEnableVertexAttribArray(1); //Col
	glEnableVertexAttribArray(2); //Norm
	glEnableVertexAttribArray(3); //Tangent
	glEnableVertexAttribArray(4); //BiTangent
	glEnableVertexAttribArray(5); //Tex1
	

	// tell our shaders where the information within our buffers lie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char *)0) + FBXVertex::PositionOffset);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char *)0) + FBXVertex::ColourOffset);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char *)0) + FBXVertex::NormalOffset);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char *)0) + FBXVertex::TangentOffset);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char *)0) + FBXVertex::BiNormalOffset);
	glVertexAttribPointer(5, 2, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char *)0) + FBXVertex::TexCoord1Offset);
	
	// finally, where done describing our mesh to the shader
	// we can describe the next mesh
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	
	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);

	m_modelMatrix = glm::mat4();

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f,0.25f,0.25f,1.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	m_lightPos = glm::vec4(20.f, 15.f, 0.f, 1.f);

	return true;
}

void Lighting_FBX_Normal::Update(float a_deltaTime)
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
	// rotate point
	float s = sinf(a_deltaTime * 0.2f);
	float c = cosf(a_deltaTime * 0.2f);
	glm::vec4 temp = m_lightPos;
	m_lightPos.x = temp.x * c - temp.z * s;
	m_lightPos.z = temp.x * s + temp.z * c;
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

void Lighting_FBX_Normal::Draw()
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	//bing our shader program
	glUseProgram(m_programID);
	//bind our vertex array object
	glBindVertexArray(m_vao);

	//get our shaders uniform location for our projectionViewMatrix and then use glUniformMatrix4fv to fill it with the correct data
	int projectionViewUniform = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(m_projectionMatrix * viewMatrix));

	int viewMatrixUniform = glGetUniformLocation(m_programID, "ViewMatrix");
	glUniformMatrix4fv(viewMatrixUniform, 1, false, glm::value_ptr(viewMatrix));

	//pass our camera position to our fragment shader uniform
	int cameraPosUniform = glGetUniformLocation(m_programID, "cameraPosition");
	glUniform4fv(cameraPosUniform, 1, glm::value_ptr(m_cameraMatrix[3]));


	//pass the directional light direction to our fragment shader
	glm::vec4 lightDir = glm::normalize(glm::vec4(0.f, 0.f, 0.f, 1.f) - m_lightPos);
	int lightDirUniform = glGetUniformLocation(m_programID, "lightDirection");
	glUniform4fv(lightDirUniform, 1, glm::value_ptr(lightDir));

	int lightPosUniform = glGetUniformLocation(m_programID, "lightPos");
	glUniform4fv(lightPosUniform, 1, glm::value_ptr(m_lightPos));

	int lightcolourUniform = glGetUniformLocation(m_programID, "lightColour");
	glUniform4fv(lightcolourUniform, 1, glm::value_ptr(glm::vec4(1.0f, 0.85f, 0.05f, 1.f)));

	//Draw our FBX Model

	// for each mesh in the model...
	for (unsigned int i = 0; i < m_fbxModel->getMeshCount(); ++i)
	{
		// get the current mesh
		FBXMeshNode *pMesh = m_fbxModel->getMeshByIndex(i);

		//send the models lighting data across to the shader
		//model Ambient
		unsigned int modelAmbient = glGetUniformLocation(m_programID, "modelAmbient");
		glUniform4fv(modelAmbient, 1, glm::value_ptr(pMesh->m_material->ambient));
		//model diffuse
		unsigned int modelDiffuse = glGetUniformLocation(m_programID, "modelDiffuse");
		glUniform4fv(modelDiffuse, 1, glm::value_ptr(pMesh->m_material->diffuse));
		//model specular
		unsigned int modelSpecular = glGetUniformLocation(m_programID, "modelSpecular");
		glUniform4fv(modelSpecular, 1, glm::value_ptr(pMesh->m_material->specular));

		glm::vec3 rafValues = glm::vec3(0.2f, 0.7f, 0.1f);
		if (strcmp("ground1", pMesh->m_material->name) == 0)
		{
			rafValues.x = 1.f;
			rafValues.y = 0.f;
			rafValues.z = 0.f;
		}

		unsigned int rafUniform = glGetUniformLocation(m_programID, "RAF");
		glUniform3fv(rafUniform, 1, glm::value_ptr(rafValues));

		// send the Model
		glm::mat4 m4Model = pMesh->m_globalTransform;// *m_modelMatrix;
		int modelUniform = glGetUniformLocation(m_programID, "Model");
		glUniformMatrix4fv(modelUniform, 1, false, glm::value_ptr(m4Model));

		//send the normal matrix
		//this is only beneficial if a model has a non-uniform scale or non-orthoganal model matrix
		glm::mat4 m4Normal = glm::transpose(m4Model);// *m_modelMatrix;
		unsigned int normalMatrixUniform = glGetUniformLocation(m_programID, "NormalMatrix");
		glUniformMatrix4fv(normalMatrixUniform, 1, false, glm::value_ptr(glm::transpose(glm::inverse(viewMatrix * m4Normal))));
		
		// Bind the texture to one of the ActiveTextures
		// if your shader supported multiple textures, you would bind each texture to a new Active Texture ID here
		//bind our textureLocation variable from the shaders and set it's value to 0 as the active texture is texture 0
		int texUniformID = glGetUniformLocation(m_programID, "DiffuseTexture");
		glUniform1i(texUniformID, 0);
		//set our active texture, and bind our loaded texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, pMesh->m_material->textureIDs[FBXMaterial::DiffuseTexture]);

		//----------------------------------------------------------//
		//--Bind Normal Maps, if we have them for this model piece--//
		//Set the uniform to use TEXTURE ID 1 for the nronal texture
		int normalTexUniformLocation = glGetUniformLocation(m_programID, "NormalTexture");
		glUniform1i(normalTexUniformLocation, 1);

		//Choose the normal map to use or none if we don't have one for the mesh that we are rendering
		int useNormalTextureUniform = glGetUniformLocation(m_programID, "useNormalMap");
		bool useLeftEngineNormalMap = strcmp(pMesh->m_material->textureFilenames[FBXMaterial::DiffuseTexture], "left_engine_diff.png") == 0; //If we are currently drawing the mesh that uses the left engine texture
		bool useTurretMap = strcmp(pMesh->m_material->textureFilenames[FBXMaterial::DiffuseTexture], "turret_diff.png") == 0; //If we are currently drawing the mesh that uses the turret texture

		if(useLeftEngineNormalMap)
		{
			//Set texture in texture 1 - that is set as the normal texture
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_normalTextures[0]);

			//Set that we are using the normal texture
			glUniform1i(useNormalTextureUniform, 1);
		}else if(useTurretMap)
		{
			//Set texture in texture 1 - that is set as the normal texture
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_normalTextures[1]);

			glUniform1i(useNormalTextureUniform, 1);
		}else
		{
			//We don't have a normal map for this section of the module - tell the shader not to use normals
			glUniform1i(useNormalTextureUniform, 0);
		}
		

		// Send the vertex data to the VBO
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, pMesh->m_vertices.size() * sizeof(FBXVertex), pMesh->m_vertices.data(), GL_STATIC_DRAW);   

		// send the index data to the IBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, pMesh->m_indices.size() * sizeof(unsigned int), pMesh->m_indices.data(), GL_STATIC_DRAW);
		
		glDrawElements(GL_TRIANGLES, pMesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
	glUseProgram(0);
	
	// draw the gizmos from this frame
	Gizmos::draw(viewMatrix, m_projectionMatrix);
	
}

void Lighting_FBX_Normal::Destroy()
{
	m_fbxModel->unload();
	delete m_fbxModel;

	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
	glDeleteVertexArrays(1, &m_vao);
	glDeleteProgram(m_programID);
	glDeleteShader(m_fragmentShader);
	glDeleteShader(m_vertexShader);

	Gizmos::destroy();
}

//Load an image in to OpenGL with from a given file path
bool Lighting_FBX_Normal::LoadImageFromFile(std::string a_filePath, unsigned& a_textureID)
{
	//Define image properties
	int width = 0, height = 0, channels = 0;

	//Set image to flip vertically on load because OpenGL reads the texture bottom to top
	stbi_set_flip_vertically_on_load(true);

	//Load Image data
	unsigned char* imageData = stbi_load(a_filePath.c_str(), &width, &height, &channels, 0);

	//Abort if image data is invalid
	if(imageData == nullptr)
	{
		Application_Log* log = Application_Log::Get();
		if(log)
		{
			log->addLog(LOG_ERROR, "Failed to load texture %s", a_filePath.c_str());
		}
		return false;
	}

	//Get a texture ID from OpenGL
	a_textureID = 0;
	glGenTextures(1, &a_textureID);

	//Set this texture to be a texture 2D and set as the active binding target - everything after this point will apply to this texture
	glBindTexture(GL_TEXTURE_2D, a_textureID);

	//Use linear mipmapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//Repeat the texture if we exceed the texture coords
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Set the texture type (R, RBG or RGBA) based on the number of channels that the image has
	GLint internalFormat;
	switch(channels)
	{
		case 1: //R
			internalFormat = GL_R;
			break;
		case 3: //RGB
			internalFormat = GL_RGB;
			break;
		case 4: //RBGA
			internalFormat = GL_RGBA;
			break;
		default:
			//error invalid number of channels in image
			return false;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, internalFormat, GL_UNSIGNED_BYTE, imageData);

	//Generate MipMap
	glGenerateMipmap(GL_TEXTURE_2D);

	//Unbind the texture
	glBindTexture(GL_TEXTURE_2D, 0);

	//Free the image from CPU memory - it's in the GPUs hands now
	stbi_image_free(imageData);
	return true;

}

