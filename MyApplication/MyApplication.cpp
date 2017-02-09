#include "MyApplication.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <gl_core_4_4.h>
#include <iostream>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

MyApplication::MyApplication()
{
}

MyApplication::~MyApplication()
{
}

bool MyApplication::startup()
{
	//Set the background colour
	setBackgroundColour(0.5f, 0.5f, 0.5f);

	//Initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	//Create virtual camera transforms
	m_viewMatrix = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(
		glm::pi<float>() * 0.25f,
		getWindowWidth() / (float)getWindowHeight(),
		0.1f, 1000.0f);

	//Create shaders
	const char* vsSource = "#version 410\n \
							layout(location=0) in vec4 position; \
							layout(location=1) in vec4 colour; \
							out vec4 vColour; \
							uniform mat4 projectionViewWorldMatrix; \
							uniform float time; \
							uniform float heightScale; \
							void main() { vColour = colour; vec4 p = position; p.y += sin(time + (position.x + position.z) / 2) * heightScale; gl_Position = projectionViewWorldMatrix * p; }";

	const char* fsSource = "#version 410\n \
							in vec4 vColour; \
							out vec4 fragColor; \
							void main() { fragColor = vColour; }";

	//Stores whether shader compilation succeeded or not
	int success = GL_FALSE;

	//Create shaders
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//Compile shaders
	//Load shader source from string (char array)
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader); 	//Compile
	//Load shader source from string (char array)
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader); //Compile

	//Link compiled shaders into shader program
	m_programID = glCreateProgram();
	glAttachShader(m_programID, vertexShader);
	glAttachShader(m_programID, fragmentShader);
	glLinkProgram(m_programID);

	//Get linking status (if it succeeded or not)
	glGetProgramiv(m_programID, GL_LINK_STATUS, &success);

	//If shader linking failed...
	if (success == GL_FALSE)
	{
		//Get log length and create char array to fit
		int infoLogLength = 0;
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		//Store log into char array
		glGetProgramInfoLog(m_programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		//Print log
		printf("%s\n", infoLog);
		
		delete[] infoLog;
	}

	//Delete individual shaders as they have been linked together
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	generateGrid();

	return true;
}
	
void MyApplication::shutdown()
{
	Gizmos::destroy();
}

void MyApplication::update(float deltaTime)
{
	m_time += deltaTime;

	//Update camera
	camera.Update(deltaTime);

	//Gizmos
	Gizmos::clear();

	//Draw a simple grid with gizmos
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, 10),
			vec3(-10 + i, 0, -10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i),
			vec3(-10, 0, -10 + i),
			i == 10 ? white : black);
	}
}

void MyApplication::draw()
{
	//Wipe screen
	clearScreen();

	//Update perspective in case window resized
	m_projectionMatrix = camera.GetProjectionMatrix(getWindowWidth(), getWindowHeight());
	m_viewMatrix = camera.GetViewMatrix();

	//Draw gizmos with virtual camera
	Gizmos::draw(m_projectionMatrix * m_viewMatrix);

	//Bind shader program, insturcting OpenGL which shaders to use
	glUseProgram(m_programID);
	//Pass MVP matrix into shader program
	unsigned int projectionViewUniform = glGetUniformLocation(m_programID, "projectionViewWorldMatrix");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(m_projectionMatrix * m_viewMatrix));

	//Pass in time and heightscale for animation
	unsigned int timeUniform = glGetUniformLocation(m_programID, "time");
	glUniform1f(timeUniform, m_time);
	unsigned int heightScaleUniform = glGetUniformLocation(m_programID, "heightScale");
	glUniform1f(heightScaleUniform, m_heightScale);

	//Bind VertexArrayObject
	glBindVertexArray(m_VAO);
	unsigned int indexCount = (rows - 1) * (cols - 1) * 6;

	//Draw triangles
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

void MyApplication::generateGrid()
{
	//Create matrix of vertices
	Vertex* aoVertices = new Vertex[rows * cols];

	//Generate vertices
	for (unsigned int r = 0; r < rows; ++r)
	{
		for (unsigned int c = 0; c < cols; ++c)
		{
			//Convert 2D index to 1D
			int index = r * cols + c;

			//Set vertex position in grid
			aoVertices[index].position = vec4((float)c, 0, (float)r, 1);

			//Create an arbitrary colour
			vec3 colour = vec3(sinf((c / (float)(cols - 1)) * (r / (float)(rows - 1))));
			aoVertices[index].colour = vec4(colour, 1);
		}
	}

	//Generate indices
	//Define index count based off quad count (2 triangles per quad)
	unsigned int* auiIndices = new unsigned int[(rows - 1) * (cols - 1) * 6];

	unsigned int index = 0;
	
	for (unsigned int r = 0; r < (rows - 1); ++r)
	{
		for (unsigned int c = 0; c < (cols - 1); ++c)
		{
			//Triangle 1
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);

			//Triangle 2
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			auiIndices[index++] = r * cols + (c + 1);
		}
	}

	//Generate GL buffers
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	//Generate VertexArrayObject
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	//Create and bind buffers to a vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (rows * cols) * sizeof(Vertex), aoVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Generated and fill index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (cols - 1) * 6 * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);

	//Bind vertex array to 0 - clear it
	glBindVertexArray(0);
	//After, unbind array buffer and element array buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//TODO: add more and stuff
	delete[] aoVertices;
	delete[] auiIndices;
}