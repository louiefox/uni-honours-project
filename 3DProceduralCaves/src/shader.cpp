#include "shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	// 1. get vertex/fragment code from files
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vertexShaderFile;
	std::ifstream fragmentShaderFile;

	// allow ifstream objects to throw exceptions
	vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// open files
		vertexShaderFile.open(vertexPath);
		fragmentShaderFile.open(fragmentPath);

		std::stringstream vertexShaderStream, fragmentShaderStream; // string stream variables

		// read file stream buffers into string streams
		vertexShaderStream << vertexShaderFile.rdbuf();
		fragmentShaderStream << fragmentShaderFile.rdbuf();

		// close files
		vertexShaderFile.close();
		fragmentShaderFile.close();

		// assign string streams to string variables
		vertexCode = vertexShaderStream.str();
		fragmentCode = fragmentShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		throw(e);
	}

	// convert code to c strings
	const char* vertexShaderCode = vertexCode.c_str();
	const char* fragmentShaderCode = fragmentCode.c_str();

	// 2. compile shaders
	unsigned int vertexShader, fragmentShader;

	// vertex shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
	glCompileShader(vertexShader);

	// print compile errors if any
	checkCompileErrors(vertexShader, "SHADER");

	// fragment shader
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
	glCompileShader(fragmentShader);

	// print compile errors if any
	checkCompileErrors(fragmentShader, "SHADER");

	// 3. shader program
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	// print linking errors
	checkCompileErrors(ID, "PROGRAM");

	// delete shaders
	glDeleteShader(vertexShader); // delete shader objects, no longer needed as they were passed by value above
	glDeleteShader(fragmentShader);
}

// Use the shader
void Shader::use()
{
	glUseProgram(ID);
}

// Utility uniform functions
void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setColor(const std::string& name, float r, float g, float b) const
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), r, g, b, 1.0f);
}

void Shader::setMat4(const std::string& name, glm::mat4 value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

// Compile errors
void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
	int success;
	char infoLog[1024];
	if (type == "SHADER")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}