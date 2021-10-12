#include "Week02_EdgeDetection.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>
#include <imgui.h>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif //STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


#include "Application_Log.h"

Week02_EdgeDetection::Week02_EdgeDetection()
{

}

Week02_EdgeDetection::~Week02_EdgeDetection()
{

}

bool Week02_EdgeDetection::LoadImageFromFile(std::string a_filePath, unsigned int& a_textureID, int& a_width,
	int& a_height)
{
	int width = 0;
	int height = 0;
	int channels = 0;

	stbi_set_flip_vertically_on_load(true); //Flip the image on load because OpenGL reads the image from the top to the bottom

	unsigned char* imageData = stbi_load(a_filePath.c_str(), &width, &height, &channels, 4);

	//Early out if data is not valid
	if(imageData == nullptr)
	{
		Application_Log* log = Application_Log::Create();

		if(log != nullptr)
		{
			log->addLog(LOG_ERROR, "Failed to open image %s", a_filePath.c_str());
		}
		return false;
	}

	a_textureID = 0;
	a_width = width;
	a_height = height;

	//Generate texture - same as generating a buffer
	glGenTextures(1, &a_textureID);
	glBindTexture(GL_TEXTURE_2D, a_textureID); //bind we are now editing the parameters of this texture

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //Linear belnd across the screen
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); //Use Linear MipMap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //Repeat the texture if we excede the texture coords
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); //Repeat the texture if we excede the texture coords

	//fill the image data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, GL_FALSE, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D); //generate mipmaps

	//Free data for the image on CPU side now it's loaded to the GPU
	stbi_image_free(imageData);

	glBindTexture(GL_TEXTURE_2D, 0); //unbind
}

bool Week02_EdgeDetection::onCreate()
{
	//Load in shaders
	m_vertexShaderID = Utility::loadShader("../shaders/vertex.glsl", GL_VERTEX_SHADER);
	m_fragmentShaderID = Utility::loadShader("../shaders/fragment.glsl", GL_FRAGMENT_SHADER);

	//Define shader pipeline inputs
	const char* szInputs[] = { "Position", "Colour", "TexCoord"};
	//Define shader pipeline outputs
	const char* szOutputs[] = { "fragColour" };
	//link shaders together
	m_shaderProgramID = Utility::createProgram(m_vertexShaderID, 0, 0, 0, m_fragmentShaderID, 3, szInputs, 1, szOutputs);

	//Create some vertex data, one vert in each corner of the screen
	Vertex* verts = new Vertex[4];
	verts[0].position = glm::vec4(-1.f, -1.f, 0.f, 1.f);
	verts[1].position = glm::vec4(-1.f, 1.f, 0.f, 1.f);
	verts[2].position = glm::vec4(1.f, -1.f, 0.f, 1.f);
	verts[3].position = glm::vec4(1.f, 1.f, 0.f, 1.f);

	verts[0].colour = glm::vec4(1.f, 0.f, 0.f, 1.f);
	verts[1].colour = glm::vec4(0.f, 1.f, 0.f, 1.f);
	verts[2].colour = glm::vec4(0.f, 0.f, 1.f, 1.f);
	verts[3].colour = glm::vec4(1.f, 1.f, 1.f, 1.f);

	verts[0].uvs = glm::vec2(0.f, 0.f);
	verts[1].uvs = glm::vec2(0.f, 1.f);
	verts[2].uvs = glm::vec2(1.f, 0.f);
	verts[3].uvs = glm::vec2(1.f, 1.f);

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

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)0 + 32 + 16);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)0 + 32);


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

	//Load texture
	bool success = LoadImageFromFile("../images/sponza.jpg", m_textureID, m_imageWidth, m_imageHeight);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f,0.25f,0.25f,1.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	return true;
}

void Week02_EdgeDetection::Update(float a_deltaTime)
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

void Week02_EdgeDetection::Draw()
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_shaderProgramID);
	glBindVertexArray(m_vao);

	//Get thee texture uniform location
	int texUniformLocation = glGetUniformLocation(m_shaderProgramID, "u_Texture");
	glUniform1i(texUniformLocation, 0);

	float widthUniformLocation = glGetUniformLocation(m_shaderProgramID, "u_Width");
	glUniform1f(widthUniformLocation, m_windowWidth);

	float heightUniformLocation = glGetUniformLocation(m_shaderProgramID, "u_Height");
	glUniform1f(heightUniformLocation, m_windowHeight);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	glDrawElements(GL_TRIANGLES, 6 /*6 verts*/, GL_UNSIGNED_INT, 0 /*start at 0 in the indicies buffer*/);

	glBindVertexArray(0);
	glUseProgram(0);
}

void Week02_EdgeDetection::Destroy()
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

