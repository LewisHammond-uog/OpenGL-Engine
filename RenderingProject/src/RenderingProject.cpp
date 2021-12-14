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
	pMesh = new Mesh();
	if(!pMesh->LoadMesh("../models/ruinedtank/tank.fbx"))
	{
		return false;
	}
	pMesh->m_transform->SetPosition(0, 0, 0);

	//Load Plane for water
	pWaterMesh = new Mesh();
	if (!pWaterMesh->LoadMesh("../models/plane/Plane.fbx"))
	{
		return false;
	}
	pWaterMesh->m_transform->SetPosition(0, 0, 0);
	pWaterMesh->m_transform->SetScale(30);


	pLightingProgram = new LightingProgram();
	pLightingProgram->Initialise();
	pLightingProgram->UseProgram();
	pLightingProgram->SetDiffuseTextureUnit(COLOUR_TEXTURE_INDEX);
	pLightingProgram->SetSpecularPowerTextureUnit(SPECULAR_POWER_TEXTURE_INDEX);
	pLightingProgram->SetNormalTextureUnit(NORMAL_TEXTURE_INDEX);
	pLightingProgram->SetShadowTextureUnit(SHADOW_TEXTURE_INDEX);

	pLightingManager = new LightingManager(pLightingProgram);
	m_pShadowSourceLight = pLightingManager->CreateDirectionalLight();
	pLightingManager->CreatePointLight();
	pLightingManager->CreatePointLight();
	pLightingManager->CreateSpotLight();

	pShadowProgram = new ShadowProgram();
	pShadowProgram->Initialise();

	pFBO = new ShadowFBO();
	pFBO->Init(1920, 1080);

	pWaterProgram = new WaterProgram();
	pWaterProgram->Initialise();

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
		ImTextureID texID = (void*)(intptr_t)pFBO->m_depthTexture;
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
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );

	glm::vec3 lightPos = glm::vec3(0, 0, 0);
	if (m_pShadowSourceLight != nullptr) 
	{
		constexpr float lightProjectionDistance = 20.f;
		lightPos = -m_pShadowSourceLight->m_worldDirection * lightProjectionDistance;
	}

	// Matrices needed for the light's perspective
	glm::mat4 orthgonalProjection = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, -100.f, 100.f);
	glm::mat4 lightView = glm::lookAt(glm::vec3(lightPos), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightProjectionView = orthgonalProjection * lightView * pMesh->m_transform->GetMatrix();

	// draw the gizmos from this frame
	Gizmos::draw(viewMatrix, m_projectionMatrix);

	//-----------------------------------------------------
	//SHADOW
	//-----------------------------------------------------
	const int shadowMapWidth = 1920;
	const int shadowMapHeight = 1080;

	pShadowProgram->UseProgram();
	pShadowProgram->SetLightViewPoint(lightProjectionView);
	pFBO->BindForWriting();
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, shadowMapWidth, shadowMapHeight);

	pMesh->Render(GL_TRIANGLES);
	//Unbund FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//-----------------------------------------------------
	//NORMAL
	//-----------------------------------------------------

	pLightingProgram->UseProgram();
	pFBO->BindForReading();

	//Set positions/materials for rendering
	glDisable(GL_BLEND);
	glm::mat4 modelWVP = m_projectionMatrix * viewMatrix * pMesh->m_transform->GetMatrix();
	pLightingProgram->SetWorldViewPoint(modelWVP);
	pLightingProgram->SetMaterial(pMesh->GetMaterial());
	pLightingProgram->SetCameraLocalPos(pMesh->m_transform->WorldDirToLocalDir(m_cameraMatrix[3]));
	pLightingProgram->SetLightViewPoint(lightProjectionView);

	pLightingManager->Update(*pMesh->m_transform);
	pLightingManager->RenderImguiWindow();

	pMesh->Render();


	glm::mat4 waterWVP = m_projectionMatrix * viewMatrix * pWaterMesh->m_transform->GetMatrix();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	pWaterProgram->UseProgram();
	pWaterProgram->SetWorldViewPoint(waterWVP);
	pWaterProgram->SetWorldCameraPos(m_cameraMatrix[3]);
	pWaterProgram->SetTime(Utility::getTotalTime());
	pWaterMesh->Render(GL_TRIANGLES);

	//Unbind Program
	glUseProgram(0);
}

void RenderingProject::Destroy()
{
	delete pLightingProgram;
	delete pMesh;
	delete pLightingManager;
	Gizmos::destroy();
}

