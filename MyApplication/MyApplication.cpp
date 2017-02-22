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

unsigned int m_staticShaderID;
unsigned int m_animatedShaderID;

Model staticModel;
Model animatedModel;

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

	//Load and compile shaders from file
	m_staticShaderID = Shader::CompileShaders("shaders/LitShader.vert", "shaders/LitShader.frag");
	m_animatedShaderID = Shader::CompileShaders("shaders/AnimatedLitShader.vert", "shaders/LitShader.frag");
	
	//Load models from file
	animatedModel.Load("models/Pyro/pyro.fbx");
	animatedModel.LoadTexture("models/Pyro/Pyro_D.tga", 0);
	animatedModel.LoadTexture("models/Pyro/Pyro_N.tga", 1);
	animatedModel.LoadTexture("models/Pyro/Pyro_S.tga", 2);

	staticModel.Load("models/soulspear.obj");
	staticModel.LoadTexture("textures/soulspear_diffuse.tga", 0);
	staticModel.LoadTexture("textures/soulspear_normal.tga", 1);
	staticModel.LoadTexture("textures/soulspear_specular.tga", 2);

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

	animatedModel.Update(getTime());
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

	//ANIMATED MODEL
	{
		//Bind shader program, insturcting OpenGL which shaders to use
		glUseProgram(m_animatedShaderID);

		//Pass in time and heightscale for animation
		unsigned int timeUniform = glGetUniformLocation(m_animatedShaderID, "time");
		glUniform1f(timeUniform, m_time);
		//Pass in camera position
		unsigned int camUniform = glGetUniformLocation(m_animatedShaderID, "cameraPosition");
		glUniform4f(camUniform, camera.GetPos().x, camera.GetPos().y, camera.GetPos().z, 1);

		unsigned int lightUniform = glGetUniformLocation(m_animatedShaderID, "light");
		glUniform4f(lightUniform, sin(m_time), 0.4f, cos(m_time), 1);

		animatedModel.Draw(glm::translate(vec3(0)) * glm::scale(vec3(0.005f)), cameraMatrix, m_animatedShaderID);
	}

	//STATIC MODEL
	{
		//Bind shader program, insturcting OpenGL which shaders to use
		glUseProgram(m_staticShaderID);

		//Pass in time and heightscale for animation
		unsigned int timeUniform = glGetUniformLocation(m_staticShaderID, "time");
		glUniform1f(timeUniform, m_time);
		//Pass in camera position
		unsigned int camUniform = glGetUniformLocation(m_staticShaderID, "cameraPosition");
		glUniform4f(camUniform, camera.GetPos().x, camera.GetPos().y, camera.GetPos().z, 1);

		unsigned int lightUniform = glGetUniformLocation(m_staticShaderID, "light");
		glUniform4f(lightUniform, sin(m_time), 0.4f, cos(m_time), 1);

		staticModel.Draw(glm::translate(vec3(5, 0, 0)) * glm::scale(vec3(2)), cameraMatrix, m_staticShaderID);
	}
}