#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include "Camera.h"

class MyApplication : public aie::Application
{
public:
	MyApplication();
	virtual ~MyApplication();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void generateGrid();

protected:
	struct Vertex
	{
		glm::vec4 position;
		glm::vec4 colour;
	};

	glm::mat4 m_viewMatrix;
	glm::mat4 m_projectionMatrix;

	Camera camera;

	unsigned int rows = 20;
	unsigned int cols = 20;

	//Vertex and index buffers
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	unsigned int m_programID;

	float m_time = 0;
	float m_heightScale = 1.0f;
};

