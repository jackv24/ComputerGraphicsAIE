#include "Scene.h"
#include <Gizmos.h>

Scene::Scene()
{
	m_lightDir = glm::vec3(0, 0.75f, 0.25f);
}

Scene::~Scene()
{
}

void Scene::Draw(float windowWidth, float windowHeight, float time)
{
	glm::mat4 cameraMatrix = camera.GetCameraMatrix();

	aie::Gizmos::draw(cameraMatrix);

	for (unsigned int i = 0; i < m_instances.size(); ++i)
	{
		m_instances[i]->Draw(this, time);
	}
}