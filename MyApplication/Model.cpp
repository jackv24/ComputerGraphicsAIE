#include "Model.h"
#include <gl_core_4_4.h>

Model::Model()
{
}

Model::~Model()
{
	if (fbxFile != nullptr)
	{
		//Clean up vertex data for each mesh
		for (unsigned int i = 0; i < fbxFile->getMeshCount(); ++i)
		{
			FBXMeshNode* mesh = fbxFile->getMeshByIndex(i);

			unsigned int* glData = (unsigned int*)mesh->m_userData;

			glDeleteVertexArrays(1, &glData[0]);
			glDeleteBuffers(1, &glData[1]);
			glDeleteBuffers(1, &glData[2]);

			delete[] glData;
		}
	}
}

bool Model::Load(const char* fileName)
{
	if (strstr(fileName, ".obj") != NULL)
	{
		std::string err;
		tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fileName);
		CreateBuffersOBJ();
		return true;
	}

	if (strstr(fileName, ".fbx") != NULL)
	{
		fbxFile = new FBXFile();
		fbxFile->load(fileName);
		CreateBuffersFBX();
		return true;
	}

	return false;
}

bool Model::LoadTexture(const char* fileName, int map)
{
	//Load texture from file
	unsigned int id = Texture::LoadTexture(fileName);

	switch (map)
	{
	case 0:
		m_diffuse.SetID(id);
		break;
	case 1:
		m_normal.SetID(id);
		break;
	case 2:
		m_specular.SetID(id);
		break;
	}

	return id != 0;
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
	glActiveTexture(GL_TEXTURE0); //Diffuse
	glBindTexture(GL_TEXTURE_2D, m_diffuse.GetID());
	int loc1 = glGetUniformLocation(programID, "diffuse");
	glUniform1i(loc1, 0);

	glActiveTexture(GL_TEXTURE1); //Normal
	glBindTexture(GL_TEXTURE_2D, m_normal.GetID());
	int loc2 = glGetUniformLocation(programID, "normal");
	glUniform1i(loc2, 1);

	glActiveTexture(GL_TEXTURE2); //Specular
	glBindTexture(GL_TEXTURE_2D, m_specular.GetID());
	int loc3 = glGetUniformLocation(programID, "specular");
	glUniform1i(loc3, 2);

	if (isAnimated())
	{
		//Grab the skeleton and animation to use
		FBXSkeleton* skeleton = fbxFile->getSkeletonByIndex(0);
		skeleton->updateBones();
		int bones_location = glGetUniformLocation(programID, "bones");
		glUniformMatrix4fv(bones_location, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);
	}

	//Bind VertexArrayObjects and draw
	if (fbxFile != nullptr)
	{
		for (auto& gl : m_glInfo)
		{
			glBindVertexArray(gl.m_VAO);
			glDrawElements(GL_TRIANGLES, gl.m_index_count, GL_UNSIGNED_INT, 0);
		}
	}
	else
	{
		for (auto& gl : m_glInfo)
		{
			glBindVertexArray(gl.m_VAO);
			glDrawArrays(GL_TRIANGLES, 0, gl.m_faceCount * 3);
		}
	}
}

void Model::Update(float time)
{
	if (isAnimated())
	{
		//Grab the skeleton and animation to use
		FBXSkeleton* skeleton = fbxFile->getSkeletonByIndex(0);
		FBXAnimation* animation = fbxFile->getAnimationByIndex(0);

		//Evaluate the animation to update bones
		skeleton->evaluate(animation, time);

		for (unsigned int bone_index = 0; bone_index < skeleton->m_boneCount; ++bone_index)
			skeleton->m_nodes[bone_index]->updateGlobalTransform();
	}
}

void Model::CreateBuffersOBJ()
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

void Model::CreateBuffersFBX()
{
	m_glInfo.resize(fbxFile->getMeshCount());

	//Create OpenGL VAO/VBO/IBO data for each mesh
	for (unsigned int i = 0; i < fbxFile->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbxFile->getMeshByIndex(i);

		//Storage for OpenGL data
		unsigned int* glData = new unsigned int[3];

		//Create buffers
		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);

		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);

		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);

		glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(FBXVertex), mesh->m_vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(unsigned int), mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); //Position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);

		glEnableVertexAttribArray(1); //Normal
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);

		glEnableVertexAttribArray(2); //UVs
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);

		//Animation
		if (isAnimated())
		{
			glEnableVertexAttribArray(3); //Weights
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::WeightsOffset);
			glEnableVertexAttribArray(4); //Indices
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::IndicesOffset);
		}

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;

		m_glInfo[i].m_VAO = glData[0];
		m_glInfo[i].m_VBO = glData[1];
		m_glInfo[i].m_IBO = glData[2];
		m_glInfo[i].m_index_count = mesh->m_indices.size();
	}
}

bool Model::isAnimated()
{
	return fbxFile && fbxFile->getSkeletonCount() > 0;
}