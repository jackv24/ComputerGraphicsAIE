#pragma once

#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <vector>
#include "tiny_obj_loader.h"
#include <Texture.h>

class Model
{
public:
	Model();
	~Model();

	bool Load(const char* fileName);
	bool LoadTexture(const char* fileName);
	void Draw(glm::mat4 transform, glm::mat4 cameraMatrix, unsigned int programID);

	void CreateBuffers();

private:
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	struct GLInfo
	{
		unsigned int m_VAO;
		unsigned int m_VBO;
		unsigned int m_faceCount;
	};
	std::vector<GLInfo> m_glInfo;

	struct OBJVertex
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;
	};

	aie::Texture m_texture;
};

