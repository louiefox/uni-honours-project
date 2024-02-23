#include "line.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

Line::Line(glm::vec3 lineStart, glm::vec3 lineEnd)
{
	vertices.push_back(LineVertex{ lineStart, glm::vec3(1.0f, 0.0f, 0.0f)});
	vertices.push_back(LineVertex{ lineEnd, glm::vec3(0.0f, 1.0f, 0.0f) });

	// Create objects
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO); // bind VAO before vertex buffers and attributes

	glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind vertex buffer

	// pass address for first Vertex struct in vertices, they're sequential so the stride/offsets will function normally
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(LineVertex), &vertices[0], GL_STATIC_DRAW); // GL_STATIC_DRAW means data will not change, GL_DYNAMIC_DRAW would store the vertices in faster memory so the data can be changed frequenetly

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)0); // attrib index, attrib size, attrib type, false, stride, offset
	glEnableVertexAttribArray(0);

	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)offsetof(LineVertex, Color)); // offsetof finds the offset of the variable within that struct
	glEnableVertexAttribArray(1);

	// we're done with VBO so unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// unbind VAO as well, prevents other calls modifying our VAO (not that it'll happen)
	glBindVertexArray(0);
}

Line::~Line()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Line::Draw()
{
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}