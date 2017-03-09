#include "Instance.h"
#include <gl_core_4_4.h>
#include "Scene.h"
#include "StatVariables.h"
#include <algorithm>

Instance::Instance(Model* model, unsigned int shaderID, Texture* diffuse, Texture* normal, Texture* specular)
{
	//If there is a model
	if (model != nullptr)
	{
		//Save pointer to model
		m_model = model;

		//Use provided textures for this instance, otherwise use default model textures
		m_diffuse = diffuse != nullptr ? diffuse : &m_model->m_diffuse;
		m_normal = normal != nullptr ? normal : &m_model->m_normal;
		m_specular = specular != nullptr ? specular : &m_model->m_specular;
	}

	//Set default position, rotation, and scale
	m_position = glm::vec3(0);
	m_eulerAngles = glm::vec3(0);
	m_scale = glm::vec3(1);

	//Save specified shader ID
	m_shaderID = shaderID;
}

Instance::~Instance()
{
}

void Instance::SetShader(unsigned int shaderID) { m_shaderID = shaderID; }

void Instance::SetPosition(glm::vec3 position) { m_position = position; }
void Instance::SetRotation(glm::vec3 eulerAngles) { m_eulerAngles = eulerAngles; }
void Instance::SetScale(glm::vec3 scale) { m_scale = scale; }

glm::mat4 Instance::GetTransform() { return m_transform; }
glm::vec3 Instance::GetPosition() { return m_position; }

void Instance::UpdateTransform()
{
	//For converting degrees to radians
	float deg2Rad = 3.1415f / 180;

	//Construct transform matrix from individual vectors
	m_transform = glm::translate(m_position)
		* glm::rotate(m_eulerAngles.z * deg2Rad, glm::vec3(0, 0, 1))
		* glm::rotate(m_eulerAngles.y * deg2Rad, glm::vec3(0, 1, 0))
		* glm::rotate(m_eulerAngles.x * deg2Rad, glm::vec3(1, 0, 0))
		* glm::scale(m_scale);
}

void Instance::Draw(Scene* scene, float time)
{
	instanceCount++;

	glm::vec3 cameraPos = scene->camera.GetPos();
	glm::vec3 lightDir = glm::normalize(scene->m_lightDir);

	glm::mat4 cameraMatrix = scene->camera.GetCameraMatrix();
	glm::mat4 mvp = cameraMatrix * m_transform;

	if (m_model)
	{
		//Camera culling
		//Check if model is off screen
		glm::vec4 corner;
		glm::vec4 screenPos[8];
		glm::vec3 screenMin(FLT_MAX);
		glm::vec3 screenMax(-FLT_MAX);
		for (int i = 0; i < 8; ++i)
		{
			corner.x = (i & 1) ? m_model->m_minBounds.x : m_model->m_maxBounds.x;
			corner.y = (i & 2) ? m_model->m_minBounds.y : m_model->m_maxBounds.y;
			corner.z = (i & 4) ? m_model->m_minBounds.z : m_model->m_maxBounds.z;
			corner.w = 1;

			screenPos[i] = mvp * corner;
			//Perspective divide
			screenPos[i].x /= screenPos[i].w;
			screenPos[i].y /= screenPos[i].w;
			screenPos[i].z /= screenPos[i].w;

			screenMin.x = std::min(screenMin.x, screenPos[i].x);
			screenMin.y = std::min(screenMin.y, screenPos[i].y);
			screenMin.z = std::min(screenMin.z, screenPos[i].z);

			screenMax.x = std::max(screenMax.x, screenPos[i].x);
			screenMax.y = std::max(screenMax.y, screenPos[i].y);
			screenMax.z = std::max(screenMax.z, screenPos[i].z);
		}

		float edge = 1.0f;
		//Don't draw if off screen
		if (screenMax.x < -edge || screenMin.x > edge ||
			screenMax.y < -edge || screenMin.y > edge ||
			screenMax.z < -edge || screenMin.z > edge)
			return;

		//Bind shader program, insturcting OpenGL which shaders to use
		glUseProgram(m_shaderID);

		//Pass in time and heightscale for animation
		unsigned int timeUniform = glGetUniformLocation(m_shaderID, "time");
		glUniform1f(timeUniform, time);

		//Pass in camera position
		unsigned int camUniform = glGetUniformLocation(m_shaderID, "cameraPosition");
		glUniform4f(camUniform, cameraPos.x, cameraPos.y, cameraPos.z, 1);

		unsigned int lightUniform = glGetUniformLocation(m_shaderID, "light");
		glUniform4f(lightUniform, lightDir.x, lightDir.y, lightDir.z, 1);


		//Draw model
		UpdateTransform();
		m_model->Update(time);
		m_model->Draw(m_transform, cameraMatrix, m_shaderID, m_diffuse, m_normal, m_specular);
	}
}