#pragma once

#include <vector>
#include "Instance.h"
#include "Camera.h"

class Scene
{
public:
	Scene();
	~Scene();

	void Draw(int windowWidth, int windowHeight);

	std::vector<Instance*> m_instances;

	Camera camera;

	glm::vec3 m_lightDir;

	float m_time = 0;
};

