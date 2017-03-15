#pragma once

#include "Scene.h"
#include <gl_core_4_4.h>
#include "Model.h"

class Framebuffer
{
public:
	Framebuffer(int width, int height);
	~Framebuffer();

	void SetUp();
	void RenderScene(Scene &scene);
	void Draw(unsigned int shaderID);

	unsigned int m_fbo;
	unsigned int m_fboTexture;
	unsigned int m_fboDepth;

	int m_width, m_height;

	Model* m_model;

	glm::vec3 m_clearColour = glm::vec3(0.25f, 0.25f, 0.25f);
};

