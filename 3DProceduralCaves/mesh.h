#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex
{
	glm::vec3 Position;
	glm::vec2 TextureCoords;
};

class Mesh
{
public:
	unsigned int VBO, VAO, EBO;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indicies;

	Mesh()
	{

	}

	~Mesh()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	void AddVertex(const glm::vec3& position, const glm::vec2 uvCoords)
	{
		vertices.push_back(Vertex{ position, uvCoords });
		indicies.push_back(vertices.size() - 1);
	}

	void Generate()
	{
		if (generated)
			return;

		generated = true;

		// Create objects
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO); // bind VAO before vertex buffers and attributes

		glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind vertex buffer

		// pass address for first Vertex struct in vertices, they're sequential so the stride/offsets will function normally
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW); // GL_STATIC_DRAW means data will not change, GL_DYNAMIC_DRAW would store the vertices in faster memory so the data can be changed frequenetly

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(unsigned int), &indicies[0], GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // attrib index, attrib size, attrib type, false, stride, offset
		glEnableVertexAttribArray(0);

		// texture coords attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TextureCoords)); // offsetof finds the offset of the variable within that struct
		glEnableVertexAttribArray(1);

		// we're done with VBO so unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// unbind VAO as well, prevents other calls modifying our VAO (not that it'll happen)
		glBindVertexArray(0);
	}

	void Draw()
	{
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

private:
	bool generated = false;
};