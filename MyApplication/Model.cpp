#include "Model.h"
#include <gl_core_4_4.h>

Model::Model()
{
}

Model::~Model()
{
}

bool Model::Load(const char* fileName)
{
	std::string err;
	return tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fileName);
}

bool Model::LoadTexture(const char* fileName)
{
	//Load texture from file
	return m_texture.load(fileName);
}

void Model::Draw(glm::mat4 transform, glm::mat4 cameraMatrix, unsigned int programID)
{
	glm::mat4 mvp = cameraMatrix * transform;

	//Pass MVP matrix into shader program
	unsigned int projectionViewUniform = glGetUniformLocation(programID, "MVP");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(mvp));
	//Pass model M matrix into shader program seperately
	unsigned int modelUniform = glGetUniformLocation(programID, "M");
	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, (float*)&transform);

	//Texturing
	//Set texture slot
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture.getHandle());
	//Tell shader where it is
	int loc = glGetUniformLocation(programID, "diffuse");
	glUniform1i(loc, 0);

	//Bind VertexArrayObjects and draw
	for (auto& gl : m_glInfo)
	{
		glBindVertexArray(gl.m_VAO);
		glDrawArrays(GL_TRIANGLES, 0, gl.m_faceCount * 3);
	}
}

void Model::CreateBuffers()
{
	m_glInfo.resize(shapes.size());

	int shapeIndex = 0;

	for (auto& shape : shapes)
	{
		//Setup OpenGL data
		glGenVertexArrays(1, &m_glInfo[shapeIndex].m_VAO);
		glGenBuffers(1, &m_glInfo[shapeIndex].m_VBO);
		glBindVertexArray(m_glInfo[shapeIndex].m_VAO);
		m_glInfo[shapeIndex].m_faceCount = shape.mesh.num_face_vertices.size();

		//Collect triangle vertices
		std::vector<OBJVertex> vertices;

		int index = 0;
		for (auto face : shape.mesh.num_face_vertices)
		{
			for (int i = 0; i < 3; ++i)
			{
				tinyobj::index_t idx = shape.mesh.indices[index + i];

				OBJVertex v = { 0 };

				//Positions
				v.x = attrib.vertices[3 * idx.vertex_index + 0];
				v.y = attrib.vertices[3 * idx.vertex_index + 1];
				v.z = attrib.vertices[3 * idx.vertex_index + 2];

				//Normals
				if (attrib.normals.size() > 0)
				{
					v.nx = attrib.normals[3 * idx.normal_index + 0];
					v.ny = attrib.normals[3 * idx.normal_index + 1];
					v.nz = attrib.normals[3 * idx.normal_index + 2];
				}

				//Texture coordinates
				if (attrib.texcoords.size() > 0)
				{
					v.u = attrib.texcoords[2 * idx.texcoord_index + 0];
					v.v = attrib.texcoords[2 * idx.texcoord_index + 1];
				}

				vertices.push_back(v);
			}

			index += face;
		}

		//Bind vertex data
		glBindBuffer(GL_ARRAY_BUFFER, m_glInfo[shapeIndex].m_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(OBJVertex), vertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0); //Position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), 0);
		glEnableVertexAttribArray(1); //Normal data
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(OBJVertex), (void*)12);
		glEnableVertexAttribArray(2); //Tex coords
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), (void*)24);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		shapeIndex++;
	}
}