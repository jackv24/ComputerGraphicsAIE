#include "Camera.h"
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <Input.h>

const float deg2Rad = 3.14159f / 180.0f;

int lastMouseX = 0;
int lastMouseY = 0;

float moveSpeed = 5.0f;
float cameraSpeed = 2.0f;

Camera::~Camera()
{
}

void Camera::Update(float deltaTime)
{
	//Get input instance
	aie::Input* input = aie::Input::getInstance();

	//Convert angles to radians
	float thetaR = theta * deg2Rad;
	float phiR = phi * deg2Rad;

	//Calculate forward and right axis and up axis for camera
	glm::vec3 forward(cos(phiR) * cos(thetaR), sin(phiR), cos(phiR) * sin(thetaR));
	glm::vec3 right(-sin(thetaR), 0, cos(thetaR));
	glm::vec3 up(0, 1, 0);

	//Use WASD, QE keys to move around
	if (input->isKeyDown(aie::INPUT_KEY_E))
		position += up * deltaTime * moveSpeed;
	if (input->isKeyDown(aie::INPUT_KEY_Q))
		position += -up * deltaTime * moveSpeed;

	if (input->isKeyDown(aie::INPUT_KEY_A))
		position += -right * deltaTime * moveSpeed;
	if (input->isKeyDown(aie::INPUT_KEY_D))
		position += right * deltaTime * moveSpeed;

	if (input->isKeyDown(aie::INPUT_KEY_W))
		position += forward * deltaTime * moveSpeed;
	if (input->isKeyDown(aie::INPUT_KEY_S))
		position += -forward * deltaTime * moveSpeed;

	//Get current mouse position
	float mx = input->getMouseX();
	float my = input->getMouseY();

	//If right mouse button is down, increment theta and pi by mouse movement
	if (input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_RIGHT))
	{
		theta += 0.05f * (mx - lastMouseX) * cameraSpeed;
		phi += 0.05f * (my - lastMouseY) * cameraSpeed;
	}

	//Store this frame's mouse pos for next frame
	lastMouseX = mx;
	lastMouseY = my;
}

glm::mat4 Camera::GetProjectionMatrix(float w, float h)
{
	return glm::perspective(glm::pi<float>() * 0.25f, w / h, 0.1f, 1000.0f);
}

glm::mat4 Camera::GetViewMatrix()
{
	//Calculate angles in radians
	float thetaR = theta * deg2Rad;
	float phiR = phi * deg2Rad;

	//Calculate local forward vector
	glm::vec3 forward(cos(phiR) * cos(thetaR), sin(phiR), cos(phiR) * sin(thetaR));

	//Return view matrix
	return glm::lookAt(position, position + forward, glm::vec3(0, 1, 0));
}