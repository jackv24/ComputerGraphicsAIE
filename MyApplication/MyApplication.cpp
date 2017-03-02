#include "MyApplication.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <gl_core_4_4.h>
#include <iostream>
#include "Shader.h"
#include "Model.h"
#include "Instance.h"
#include "Scene.h"
#include <imgui.h>
#include "Framebuffer.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

struct Vertex
{
	glm::vec4 position;
	glm::vec4 colour;
};

unsigned int m_staticShaderID;
unsigned int m_animatedShaderID;

Scene scene;

Model* staticModel;
Model* animatedModel;

Texture* testDiffuse;
Texture* testNormal;
Texture* testSpecular;

Framebuffer* frameBuffer;
Model* quadModel;
unsigned int m_postProcessShaderID;

MyApplication::MyApplication()
{
}

MyApplication::~MyApplication()
{
	delete staticModel;
	delete animatedModel;

	delete testDiffuse;
	delete testNormal;
	delete testSpecular;

	delete frameBuffer;
	delete quadModel;
}

bool MyApplication::startup()
{
	//Set the background colour
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	//Initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	//Setup frame buffer
	frameBuffer = new Framebuffer(getWindowWidth(), getWindowHeight());
	frameBuffer->SetUp();

	//Setup quad for rendering framebuffer to screen
	quadModel = new Model();
	quadModel->MakePostProcessQuad(getWindowWidth(), getWindowHeight());
	frameBuffer->m_model = quadModel;
	m_postProcessShaderID = Shader::CompileShaders("shaders/PostProcessBase.vert", "shaders/PostProcessBase.frag");

	//Load and compile shaders from file
	m_staticShaderID = Shader::CompileShaders("shaders/LitShader.vert", "shaders/LitShader.frag");
	m_animatedShaderID = Shader::CompileShaders("shaders/AnimatedLitShader.vert", "shaders/LitShader.frag");
	
	//Load textures
	testDiffuse = new Texture();
	testDiffuse->SetID(Texture::LoadTexture("textures/numbered_grid.tga"));
	testNormal = new Texture();
	testNormal->SetID(Texture::LoadTexture("textures/NormalMap.png"));
	testSpecular = new Texture();
	testSpecular->SetID(Texture::LoadTexture("textures/SpecularMap.png"));

	//Load models from file
	staticModel = new Model();
	staticModel->Load("models/soulspear.obj");
	staticModel->LoadTexture("textures/soulspear_diffuse.tga", 0);
	staticModel->LoadTexture("textures/soulspear_normal.tga", 1);
	staticModel->LoadTexture("textures/soulspear_specular.tga", 2);

	animatedModel = new Model();
	animatedModel->Load("models/Pyro/pyro.fbx");
	animatedModel->LoadTexture("models/Pyro/Pyro_D.tga", 0);
	animatedModel->LoadTexture("models/Pyro/Pyro_N.tga", 1);
	animatedModel->LoadTexture("models/Pyro/Pyro_S.tga", 2);

	//Create instances
	Instance* inst;

	inst = new Instance(staticModel, m_staticShaderID, nullptr, nullptr, nullptr);
	inst->SetPosition(vec3(0));
	scene.m_instances.push_back(inst);

	inst = new Instance(staticModel, m_staticShaderID, testDiffuse, testNormal, testNormal);
	inst->SetPosition(vec3(3, 0, 0));
	scene.m_instances.push_back(inst);

	inst = new Instance(staticModel, Shader::CompileShaders("shaders/WaveShader.vert", "shaders/LitShader.frag"), nullptr, nullptr, nullptr);
	inst->SetPosition(vec3(6, 0, 0));
	scene.m_instances.push_back(inst);

	inst = new Instance(animatedModel, m_animatedShaderID, nullptr, nullptr, nullptr);
	inst->SetPosition(vec3(-5, 0, 0));
	inst->SetScale(vec3(0.005));
	scene.m_instances.push_back(inst);

	return true;
}
	
void MyApplication::shutdown()
{
	Gizmos::destroy();
}

void MyApplication::update(float deltaTime)
{
	m_time += deltaTime;

	//Update scene time
	scene.m_time = m_time;

	//Update camera
	scene.camera.Update(getWindowWidth(), getWindowHeight(), deltaTime);

	//Gizmos
	Gizmos::clear();

	//Draw a simple grid with gizmos
	vec4 white(1);
	vec4 black(0.1f, 0.1f, 0.1f, 1);
	for (int i = 0; i < 21; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, 10),
			vec3(-10 + i, 0, -10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i),
			vec3(-10, 0, -10 + i),
			i == 10 ? white : black);
	}
}

void MyApplication::draw()
{
	//Wipe screen
	clearScreen();

	//Draw UI
	ImGui::Begin("Light");
	ImGui::SliderFloat3("Light Direction", &scene.m_lightDir.x, -1, 1);
	ImGui::End();

	//Draw scene stuff and gizmos
	float screenWidth = getWindowWidth();
	float screenHeight = getWindowHeight();
	float time = getTime();

	//Draw gizmos with virtual camera
	Gizmos::draw(scene.camera.GetCameraMatrix());

	//Draw framebuffer
	frameBuffer->RenderScene(scene);
	//Draw scene
	frameBuffer->Draw(m_postProcessShaderID);
}