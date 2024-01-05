#pragma once

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

struct LineVertex
{
	glm::vec3 Position;
	glm::vec3 Color;
};

class Line
{
public:
	unsigned int VBO, VAO;
	std::vector<LineVertex> vertices;

	Line(glm::vec3 lineStart, glm::vec3 lineEnd);
	~Line();

	void Draw();

private:
};