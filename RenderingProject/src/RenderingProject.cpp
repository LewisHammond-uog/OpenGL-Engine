#include "RenderingProject.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>
#include <imgui.h>

//Project Includes
#include "Macros.h"
#include "Application_Log.h"
#include "WorldTransform.h"
#include "Mesh.h"
#include "DirectionalLight.h"
#include "LightingProgram.h"
#include "LightingManager.h"
#include "ShadowProgram.h"
#include "ShadowFBO.h"
#include "WaterProgram.h"


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
	m_pSceneMesh = new Mesh();
	if(!m_pSceneMesh->LoadMesh("../models/ruinedtank/tank.fbx"))
	{
		return false;
	}
	m_pSceneMesh->m_transform->SetPosition(0, 0, 0);

	//Load Plane for water
	m_pWaterMesh = new Mesh();
	if (!m_pWaterMesh->LoadMesh("../models/plane/Plane.fbx"))
	{
		return false;
	}
	m_pWaterMesh->m_transform->SetPosition(0, 0, 0);
	m_pWaterMesh->m_transform->SetScale(30);


	m_pLightingProgram = new LightingProgram();
	m_pLightingProgram->Initialise();
	m_pLightingProgram->UseProgram();
	m_pLightingProgram->SetDiffuseTextureUnit(COLOUR_TEXTURE_INDEX);
	m_pLightingProgram->SetSpecularPowerTextureUnit(SPECULAR_POWER_TEXTURE_INDEX);
	m_pLightingProgram->SetNormalTextureUnit(NORMAL_TEXTURE_INDEX);
	m_pLightingProgram->SetShadowTextureUnit(SHADOW_TEXTURE_INDEX);

	m_pLightingManager = new LightingManager(m_pLightingProgram);
	m_pDirecitonalSourceLight = m_pLightingManager->CreateDirectionalLight();
	m_pLightingManager->CreatePointLight();
	m_pLightingManager->CreatePointLight();
	m_pLightingManager->CreateSpotLight();

	m_pShadowProgram = new ShadowProgram();
	m_pShadowProgram->Initialise();

	m_pFBO = new ShadowFBO();
	m_pFBO->Init(1920, 1080);

	m_pWaterProgram = new WaterProgram();
	m_pWaterProgram->Initialise();

	//Set the max verts of patches
	GLint MaxPatchVertices = 0;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
	glPatchParameteri(GL_PATCH_VERTICES, 3);

	//Set the polygon mode to fill (used for wire frame mode)
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	return true;
}

void RenderingProject::Update(float a_deltaTime)
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

	ImGui::Begin("Depth Buffer");
	ImGui::BeginTabBar("FBO");
	if (ImGui::BeginTabItem("Depth Buffer")) {
		ImTextureID texID = (void*)(intptr_t)m_pFBO->m_depthTexture;
		ImGui::Image(texID, ImVec2(m_windowWidth * 0.25, m_windowHeight * 0.25), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::EndTabItem();
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
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void RenderingProject::Draw()
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	m_viewMatrix = glm::inverse( m_cameraMatrix );

	//Calculate the light position
	glm::vec3 lightPos = glm::vec3(0, 0, 0);
	if (m_pDirecitonalSourceLight != nullptr) 
	{
		constexpr float lightProjectionDistance = 20.f;
		lightPos = -m_pDirecitonalSourceLight->m_v3WorldDirection * lightProjectionDistance;
	}

	// Matrices needed for the light's perspective
	glm::mat4 orthgonalProjection = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, -100.f, 100.f);
	glm::mat4 lightView = glm::lookAt(glm::vec3(lightPos), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightProjectionView = orthgonalProjection * lightView * m_pSceneMesh->m_transform->GetMatrix();

	ShadowPass(lightProjectionView);
	RenderPass(lightProjectionView);
	WaterPass();



	//Unbind Program
	glUseProgram(0);
}


void RenderingProject::WaterPass()
{
	if(!m_pWaterProgram)
	{
		return;
	}

	glm::mat4 waterWVP = m_projectionMatrix * m_viewMatrix * m_pWaterMesh->m_transform->GetMatrix();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	m_pWaterProgram->UseProgram();
	m_pWaterProgram->SetWorldViewPoint(waterWVP);
	m_pWaterProgram->SetWorldCameraPos(m_cameraMatrix[3]);
	m_pWaterProgram->SetTime(Utility::getTotalTime());
	m_pWaterProgram->SetLightDirection(m_pDirecitonalSourceLight->m_v3WorldDirection);
	m_pWaterProgram->SetLightColour(m_pDirecitonalSourceLight->m_v3LightColour);
	m_pWaterMesh->Render(GL_TRIANGLES);
}

void RenderingProject::ShadowPass(const glm::mat4 a_lightProjectionView)
{
	const int shadowMapWidth = 1920;
	const int shadowMapHeight = 1080;

	if (!m_pShadowProgram || !m_pFBO)
	{
		return;
	}

	m_pShadowProgram->UseProgram();
	m_pShadowProgram->SetLightViewPoint(a_lightProjectionView);
	m_pFBO->BindForWriting();
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, shadowMapWidth, shadowMapHeight);

	m_pSceneMesh->Render(GL_TRIANGLES);
	//Unbund FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderingProject::RenderPass(const glm::mat4 a_lightProjectionView)
{
	if (!m_pShadowProgram || !m_pFBO)
	{
		return;
	}

	m_pLightingProgram->UseProgram();
	m_pFBO->BindForReading();

	//Set positions/materials for rendering
	glDisable(GL_BLEND);
	glm::mat4 modelWVP = m_projectionMatrix * m_viewMatrix * m_pSceneMesh->m_transform->GetMatrix();
	m_pLightingProgram->SetWorldViewPoint(modelWVP);
	m_pLightingProgram->SetMaterial(m_pSceneMesh->GetMaterial());
	m_pLightingProgram->SetCameraLocalPos(m_pSceneMesh->m_transform->WorldDirToLocalDir(m_cameraMatrix[3]));
	m_pLightingProgram->SetLightViewPoint(a_lightProjectionView);

	m_pLightingManager->Update(*m_pSceneMesh->m_transform);
	m_pLightingManager->RenderImguiWindow();

	m_pSceneMesh->Render();
}

void RenderingProject::Destroy()
{
	delete m_pLightingProgram;
	delete m_pSceneMesh;
	delete m_pLightingManager;
	Gizmos::destroy();
}


