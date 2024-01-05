#pragma once

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <string>

class Shader
{
public:
	unsigned int ID;

	// Constructor reads and builds shader
	Shader(const char* vertexPath, const char* fragmentPath);

	// Use/activate shader
	void use();

	// Utility uniform functions
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setColor(const std::string& name, float r, float g, float b) const;
	void setMat4(const std::string& name, glm::mat4 value) const;

private:
	void checkCompileErrors(unsigned int shader, std::string type);
};