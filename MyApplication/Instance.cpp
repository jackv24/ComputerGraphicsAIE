#include "Instance.h"
#include <gl_core_4_4.h>
#include "Scene.h"
#include "StatVariables.h"

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

	if (m_model)
	{
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