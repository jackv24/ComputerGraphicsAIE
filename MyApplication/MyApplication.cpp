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

Model* staticModel;

Texture* testDiffuse;
Texture* testNormal;
Texture* testSpecular;

std::vector<Instance*> instances;

MyApplication::MyApplication()
{
}

MyApplication::~MyApplication()
{
	delete staticModel;

	delete testDiffuse;
	delete testNormal;
	delete testSpecular;
}

bool MyApplication::startup()
{
	//Set the background colour
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	//Initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	//Create virtual camera transforms
	m_viewMatrix = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(
		glm::pi<float>() * 0.25f,
		getWindowWidth() / (float)getWindowHeight(),
		0.1f, 1000.0f);

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

	//Create instances
	Instance* inst;

	inst = new Instance(staticModel, m_staticShaderID, nullptr, nullptr, nullptr);
	inst->SetPosition(vec3(0));
	instances.push_back(inst);

	inst = new Instance(staticModel, m_staticShaderID, testDiffuse, testNormal, testNormal);
	inst->SetPosition(vec3(3, 0, 0));
	instances.push_back(inst);

	inst = new Instance(staticModel, Shader::CompileShaders("shaders/WaveShader.vert", "shaders/LitShader.frag"), nullptr, nullptr, nullptr);
	inst->SetPosition(vec3(6, 0, 0));
	instances.push_back(inst);

	return true;
}
	
void MyApplication::shutdown()
{
	Gizmos::destroy();
}

void MyApplication::update(float deltaTime)
{
	m_time += deltaTime;

	//Update camera
	camera.Update(deltaTime);

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

	//Update perspective in case window resized
	m_projectionMatrix = camera.GetProjectionMatrix((float)getWindowWidth(), (float)getWindowHeight());
	m_viewMatrix = camera.GetViewMatrix();

	glm::mat4 cameraMatrix = m_projectionMatrix * m_viewMatrix;

	//Draw gizmos with virtual camera
	Gizmos::draw(m_projectionMatrix * m_viewMatrix);


	//Draw instances
	for (unsigned int i = 0; i < instances.size(); i++)
	{
		instances[i]->Draw(cameraMatrix, camera.GetPos(), getTime());
	}
}