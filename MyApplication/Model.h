#pragma once

#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <vector>
#include "tiny_obj_loader.h"
#include "Texture.h"
#include "../dependencies/FBXLoader/FBXFile.h"

class Model
{
public:
	Model();
	~Model();

	bool Load(const char* fileName);
	bool LoadTexture(const char* fileName, int map);
	void Draw(glm::mat4 transform, glm::mat4 cameraMatrix, unsigned int programID, Texture* diffuse, Texture* normal, Texture* specular);
	void Update(float time);

	void CreateBuffersOBJ();
	void CreateBuffersFBX();

	void CalculateBoundsOBJ();
	void SetBounds(glm::vec3 min, glm::vec3 max);

	bool isAnimated();

	void MakePostProcessQuad(int width, int height);
	unsigned int GetVAO();

	Texture m_diffuse;
	Texture m_normal;
	Texture m_specular;

	glm::vec3 m_minBounds, m_maxBounds;

private:
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	struct GLInfo
	{
		unsigned int m_VAO;
		unsigned int m_VBO;
		unsigned int m_IBO;
		unsigned int m_index_count;
		unsigned int m_faceCount;
	};
	std::vector<GLInfo> m_glInfo;

	struct OBJVertex
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;
	};

	FBXFile* fbxFile;
};

