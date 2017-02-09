#include "MyApplication.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm\glm.hpp>
#include <glm\ext.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

MyApplication::MyApplication()
{
}

MyApplication::~MyApplication()
{
}

bool MyApplication::startup()
{
	//Set the background colour
	setBackgroundColour(0.5f, 0.5f, 0.5f);

	//Initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	//Create virtual camera transforms
	m_viewMatrix = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(
		glm::pi<float>() * 0.25f,
		getWindowWidth() / (float)getWindowHeight(),
		0.1f, 1000.0f);

	return true;
}
	
void MyApplication::shutdown()
{
	Gizmos::destroy();
}

void MyApplication::update(float deltaTime)
{
	Gizmos::clear();

	//Draw a simple grid with gizmos
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i) {
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
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
		getWindowWidth() / (float)getWindowHeight(),
		0.1f, 1000.f);

	//Draw gizmos with virtual camera
	Gizmos::draw(m_projectionMatrix * m_viewMatrix);
}