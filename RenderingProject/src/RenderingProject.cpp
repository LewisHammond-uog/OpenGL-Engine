#include "RenderingProject.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>
#include <imgui.h>

#include "Application_Log.h"
#include "Mesh.h"
#include "LightingProgram.h"
#include "DirectionalLight.h"
#include "Macros.h"
#include "PointLight.h"
#include "WorldTransform.h"
#include "../../deps/imgui/impl/imgui_impl_glfw.h"
#include "../../deps/imgui/impl/imgui_impl_opengl3.h"

RenderingProject::RenderingProject()
{

}

RenderingProject::~RenderingProject()
{

}

bool RenderingProject::onCreate()
{
	// initialise the Gizmos helper class
	Gizmos::create();
	
	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)));
	
	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, m_windowWidth/(float)m_windowHeight, 0.1f, 1000.0f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f,0.25f,0.25f,1.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//Load mesh
	pMesh = new Mesh();
	pMesh->LoadMesh("../models/ruinedtank/tank.fbx");

	pLightingProgram = new LightingProgram();
	pLightingProgram->Initialise();
	pLightingProgram->UseProgram();
	pLightingProgram->SetDiffuseTextureUnit(COLOUR_TEXTURE_INDEX);
	pLightingProgram->SetSpecularPowerTextureUnit(SPECULAR_POWER_TEXTURE_INDEX);


	pointLights[0] = new PointLight();
	pointLights[0]->m_worldPosition = glm::vec3(0, 10, 0);
	pointLights[0]->m_diffuseIntensity = 0.0f;
	pointLights[0]->m_ambientIntensity = 1.0f;
	pointLights[0]->m_lightColour = glm::vec3(1.0f, 1.0f, 1.0f);
	pointLights[0]->m_attenuation.m_linear = 0.0f;
	pointLights[0]->m_attenuation.m_exponential = 0.1f;

	glUseProgram(0);

	//directionalLight = new DirectionalLight();
	//directionalLight->m_ambientIntensity = 0.1f;
	//directionalLight->m_diffuseIntensity = 1.f;
	//directionalLight->m_worldDirection = glm::vec3(1.0f, 0.f, 0.f);

	return true;
}

void RenderingProject::Update(float a_deltaTime)
{
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	// clear all gizmos from last frame
	Gizmos::clear();
	
	Gizmos::addBox(pointLights[0]->m_worldPosition, glm::vec3(0.3f), true, glm::vec4(pointLights[0]->m_lightColour, 1.f));
	
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

void RenderingProject::Draw()
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	// draw the gizmos from this frame
	Gizmos::draw(viewMatrix, m_projectionMatrix);

	WorldTransform* transform = new WorldTransform();
	transform->SetPosition(glm::vec3(0, 0, 0));

	pLightingProgram->UseProgram();

	//directionalLight->CalculateLocalDirection(modelWorldTransform);

	glm::mat4 worldViewProjection = m_projectionMatrix * viewMatrix * transform->GetMatrix();
	pLightingProgram->SetWorldViewPoint(worldViewProjection);
	//pLightingProgram->SetDirectionalLight(*directionalLight);
	pLightingProgram->SetMaterial(pMesh->GetMaterial());

	pointLights[0]->CalculateLocalPosition(*transform);
	//pointLights[1]->CalculateLocalPosition(*transform);
	pLightingProgram->SetPointLights(1, *pointLights);

	pLightingProgram->SetCameraLocalPos(transform->WorldDirToLocalDir(m_cameraMatrix[3]));

	pMesh->Render();

	RenderImguiWindow();

	//Unbind Program
	glUseProgram(0);
}

void RenderingProject::Destroy()
{
	//delete pLightingProgram;
	//delete pointLights;
	Gizmos::destroy();
}

void RenderingProject::RenderImguiWindow()
{
	ImGui::Begin("Setting Window");
	if (ImGui::CollapsingHeader("Lighting Settings")) { 

		if (ImGui::CollapsingHeader("Point Lights")) {
			for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(pointLights); i++) {
				if (pointLights[i] != nullptr) {


					//Position
					constexpr float posDragInterval = 0.1f;
					float* pos[3] = { &pointLights[i]->m_worldPosition.x, &pointLights[i]->m_worldPosition.y, &pointLights[i]->m_worldPosition.z };
					ImGui::DragFloat3("Position", *pos, posDragInterval);

					//Colour
					float* col[3] = { &pointLights[i]->m_lightColour.r, &pointLights[i]->m_lightColour.g, &pointLights[i]->m_lightColour.b };
					ImGui::Text("Light Colour");
					ImGui::ColorEdit3("Colour Picker", *col);

					//Fall off
					constexpr float lightFalloffInterval = 0.001f;
					ImGui::DragFloat("Constant Falloff", &pointLights[i]->m_attenuation.m_constant, lightFalloffInterval);
					ImGui::DragFloat("Linear Falloff", &pointLights[i]->m_attenuation.m_linear, lightFalloffInterval);
					ImGui::DragFloat("Exp Falloff", &pointLights[i]->m_attenuation.m_exponential, lightFalloffInterval);

					


				}
			}
		}

	}
	ImGui::End();
}

