#pragma once

#include "Application.h"
#include "Camera.h"
#include <vector>
#include <string>
#include "tiny_obj_loader.h"

class MyApplication : public aie::Application
{
public:
	MyApplication();
	virtual ~MyApplication();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:
	glm::mat4 m_viewMatrix;
	glm::mat4 m_projectionMatrix;

	Camera camera;

	unsigned int rows = 20;
	unsigned int cols = 20;

	unsigned int m_programID;

	float m_time = 0;
	float m_heightScale = 1.0f;
};

