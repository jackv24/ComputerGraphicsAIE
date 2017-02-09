#pragma once

#include <glm\mat4x4.hpp>
#include <glm\vec3.hpp>

class Camera
{
public:
	Camera() : theta(0), phi(-20), position(-10, 4, 0) {}
	~Camera();

	void Update(float deltaTime);

	glm::mat4 GetProjectionMatrix(float w, float h);
	glm::mat4 GetViewMatrix();

private:
	float theta;
	float phi;

	glm::vec3 position;
};

