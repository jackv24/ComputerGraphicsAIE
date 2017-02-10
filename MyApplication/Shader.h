#pragma once
#include <string>

class Shader
{
public:
	static unsigned int CompileShaders(const char* vsFile, const char* fsFile);
private:
	static bool LoadShaderFromFile(const char* filePath, std::string& code);
};