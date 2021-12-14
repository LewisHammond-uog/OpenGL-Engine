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


RenderingProject::RenderingProject(): m_programID(0), m_pSceneMesh(nullptr), m_pWaterMesh(nullptr),
                                      m_pDirecitonalSourceLight(nullptr),
                                      m_pLightingProgram(nullptr),
                                      m_pLightingManager(nullptr),
                                      m_pShadowProgram(nullptr), m_pFBO(nullptr),
                                      m_pWaterProgram(nullptr),
									  m_bDrawWireFrame(false),
								      m_bVsyncEnabled(true)
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

	//Initalise Shadow Texture Size
	m_iShadowMapHeight = m_windowHeight;
	m_iShadowMapWidth = m_windowWidth;

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
	m_pDirecitonalSourceLight = m_pLightingManager->CreateDirectionalLight(glm::vec3(0,-1,1),
																			glm::vec3(1.0f),
																			0.01f,
																			0.1f);
	m_pLightingManager->CreatePointLight(glm::vec3(0.f,1.1f, -14.7f),
										 glm::vec3(0.8, 0, 0.91),
										 1.2f,
											1.5f,
											0.074f,
											0.023f);
	m_pLightingManager->CreatePointLight(glm::vec3(13.6f, 1.9f, -5.1f),
										glm::vec3(0, 0.78f, 0.29f),
										1.13f,
										1.0f,
										0.1f,
										0.141f);
	m_pLightingManager->CreateSpotLight(glm::vec3(0.f, 4.0f, -6.9f), 
											glm::vec3(-0.33f, -1.f, 3.62f),
											glm::vec3(1.f),
											13.0f,
											2.0f,
											0.87f,
											0.142f,
											30.8f);

	m_pShadowProgram = new ShadowProgram();
	m_pShadowProgram->Initialise();

	m_pFBO = new ShadowFBO();
	m_pFBO->Init(m_iShadowMapWidth, m_iShadowMapHeight);

	m_pWaterProgram = new WaterProgram();
	m_pWaterProgram->Initialise();

	//Set the max verts of patches
	GLint maxPatchVertices = 0;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &maxPatchVertices);
	glPatchParameteri(GL_PATCH_VERTICES, 3);

	glUseProgram(0);

	return true;
}

void RenderingProject::Update(float a_deltaTime)
{
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	// clear all gizmos from last frame
	Gizmos::clear();
	
	//Draw Depth Buffer Window & Settings Window
	DrawDepthBufferWindow();
	DrawSceneSettingsWindow();

	//Toggle Vsync
	if(m_bVsyncEnabled)
	{
		glfwSwapInterval(1);
	}else
	{
		glfwSwapInterval(0);
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

	//Draw Gizmos
	m_pLightingManager->DrawLightingGizmos();

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

	//Set drawing mode to wireframe or not
	if(m_bDrawWireFrame)
	{

		//Set the polygon mode to fill (not wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}else
	{
		//Set the polygon mode to fill (not wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	ShadowPass(lightProjectionView);
	RenderPass(lightProjectionView);
	WaterPass();

	//Unbind Program
	glUseProgram(0);


	Gizmos::draw(m_projectionMatrix * m_viewMatrix);
}


void RenderingProject::WaterPass()
{
	if(!m_pWaterProgram)
	{
		return;
	}

	const glm::mat4 waterWVP = m_projectionMatrix * m_viewMatrix * m_pWaterMesh->m_transform->GetMatrix();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	m_pWaterProgram->UseProgram();
	m_pWaterProgram->SetWorldViewPoint(waterWVP);
	m_pWaterProgram->SetWorldCameraPos(m_cameraMatrix[3]);
	m_pWaterProgram->SetTime(Utility::getTotalTime());
	m_pWaterProgram->SetLightDirection(m_pDirecitonalSourceLight->m_v3WorldDirection);
	m_pWaterProgram->SetLightColour(m_pDirecitonalSourceLight->m_v3LightColour);
	m_pWaterMesh->Render(GL_TRIANGLES);
	glUseProgram(0);
}

void RenderingProject::ShadowPass(const glm::mat4 a_lightProjectionView)
{

	if (!m_pShadowProgram || !m_pFBO)
	{
		return;
	}

	m_pShadowProgram->UseProgram();
	m_pShadowProgram->SetLightViewPoint(a_lightProjectionView);
	m_pFBO->BindForWriting();
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, m_iShadowMapWidth, m_iShadowMapHeight);

	m_pSceneMesh->Render(GL_TRIANGLES);
	//Unbund FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Reset view port
	glViewport(0,0,m_windowWidth, m_windowHeight);
	glUseProgram(0);
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

	glUseProgram(0);
}

/// <summary>
/// Draw an Imgui Window that draws the depth buffer
/// </summary>
void RenderingProject::DrawDepthBufferWindow()
{
	ImGui::Begin("Depth Buffer");
	ImGui::BeginTabBar("FBO");
	if (ImGui::BeginTabItem("Depth Buffer")) {
		ImTextureID texID = (void*)(intptr_t)m_pFBO->m_depthTexture;
		ImGui::Image(texID, ImVec2(m_windowWidth * 0.25, m_windowHeight * 0.25), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
	ImGui::End();
}

/// <summary>
/// Draw an Imgui Window that shows the scene settings
/// </summary>
void RenderingProject::DrawSceneSettingsWindow()
{
	ImGui::Begin("Scene Settings");
	ImGui::Checkbox("Draw Wireframe", &m_bDrawWireFrame);
	ImGui::Checkbox("Vsync", &m_bVsyncEnabled);
	ImGui::End();


}

void RenderingProject::Destroy()
{
	delete m_pLightingProgram;
	delete m_pSceneMesh;
	delete m_pWaterMesh;
	delete m_pLightingManager;
	delete m_pWaterProgram;
	delete m_pFBO;
	delete m_pShadowProgram;
	Gizmos::destroy();
}


