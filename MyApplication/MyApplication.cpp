#include "MyApplication.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <gl_core_4_4.h>
#include <iostream>
#include "Shader.h"
#include "Model.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

struct Vertex
{
	glm::vec4 position;
	glm::vec4 colour;
};

Model model1;
Model model2;

MyApplication::MyApplication()
{
}

MyApplication::~MyApplication()
{
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

	//File path to load shaders from
	const char* vsFile = "shaders/LitShader.vert";
	const char* fsFile = "shaders/LitShader.frag";

	//Load and compile shaders from file
	m_programID = Shader::CompileShaders(vsFile, fsFile);
	
	//Load models from file
	model1.Load("models/Computer_Laptop.obj");
	model1.CreateBuffers();
	model1.LoadTexture("textures/Computer_Laptop_D.tga");

	model2.Load("models/sphere.obj");
	model2.CreateBuffers();
	model2.LoadTexture("textures/earth_diffuse.jpg");

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

	//Bind shader program, insturcting OpenGL which shaders to use
	glUseProgram(m_programID);

	//Pass in time and heightscale for animation
	unsigned int timeUniform = glGetUniformLocation(m_programID, "time");
	glUniform1f(timeUniform, m_time);
	unsigned int heightScaleUniform = glGetUniformLocation(m_programID, "heightScale");
	glUniform1f(heightScaleUniform, m_heightScale);
	//Pass in camera position
	unsigned int camUniform = glGetUniformLocation(m_programID, "cameraPosition");
	glUniform4f(camUniform, camera.GetPos().x, camera.GetPos().y, camera.GetPos().z, 1);

	//Draw a bunch of one model
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			model1.Draw(glm::translate(vec3(i * 2, 0, j * 2)), cameraMatrix, m_programID);
		}
	}

	//Draw one of other model
	model2.Draw(glm::translate(vec3(0, 2, 0)), cameraMatrix, m_programID);
}