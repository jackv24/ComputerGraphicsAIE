#pragma once

#include <glm\mat4x4.hpp>
#include <glm\vec3.hpp>

class Camera
{
public:
	Camera() : theta(0), phi(-20), position(-10, 4, 0) {}
	~Camera();

	void Update(float width, float height, float deltaTime);

	glm::mat4 GetProjectionMatrix();
	glm::mat4 GetViewMatrix();
	glm::mat4 GetCameraMatrix();

	glm::vec3 GetPos();

private:
	float theta;
	float phi;

	float pitchLimit = 80.0f;

	glm::vec3 position;

	float m_width = 0;
	float m_height = 0;
};

