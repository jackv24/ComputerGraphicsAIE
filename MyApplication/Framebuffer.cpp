#include "Framebuffer.h"

Framebuffer::Framebuffer(int width, int height)
{
	m_width = width;
	m_height = height;
}

Framebuffer::~Framebuffer()
{
}

void Framebuffer::SetUp()
{
	//Generate new frame buffer
	glGenFramebuffers(1, &m_fbo);
	//Set as current frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	//Generate a new texture
	glGenTextures(1, &m_fboTexture);
	//Set as current texture
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);

	//Allocate for width x height, with RGB 8 bytes each on the GPU
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, m_width, m_height);

	//Texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Attach this texture and its data to the current frame buffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fboTexture, 0);

	//Create a new depth buffer and set it as the current one
	glGenRenderbuffers(1, &m_fboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_fboDepth);

	//Allocate memory on the GPU for the depth buffer
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height);

	//Attach this depth buffer to the current frame buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fboDepth);


	//Test everything is set up correctly
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer Error!\n");

	//Done! Detach frame buffer so the current one is once again the default screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::RenderScene(Scene &scene)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glViewport(0, 0, m_width, m_height);

	glClearColor(0.25f, 0.25f, 0.25f, 1); //Maybe pass in as argument later

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw the actual scene
	scene.Draw(m_width, m_height);

	//Restore normal framebuffer after
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, m_width, m_height);

	glClearColor(0.25f, 0.25f, 0.25f, 1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Framebuffer::Draw(unsigned int shaderID)
{
	glUseProgram(shaderID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);

	int loc = glGetUniformLocation(shaderID, "target");
	glUniform1i(loc, 0);

	glBindVertexArray(m_model->GetVAO());
	
	glDrawArrays(GL_TRIANGLES, 0, 6);
}