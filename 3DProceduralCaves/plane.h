#pragma once

#include <glad/glad.h>
#include <vector>

struct Vertex
{
	glm::vec3 Position;
	glm::vec2 TextureCoords;
};

class Plane
{
public:
	unsigned int VBO, VAO, EBO;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indicies;

	Plane(unsigned int squaresCount)
	{
		unsigned int planeSquaresWidth = std::sqrt(squaresCount);
		float individualSquareSize = 1.0f / (float)planeSquaresWidth;
		for (int y = 0; y < planeSquaresWidth; y++)
		{
			for (int x = 0; x < planeSquaresWidth; x++)
			{
				glm::vec3 topLeft = glm::vec3(-0.5f + (x * individualSquareSize), -0.5f + (y * individualSquareSize), 0.0f);

				// TODO: Don't repeat vertices, also don't push indicies like this
				// tri 1
				vertices.push_back(Vertex{ topLeft, glm::vec2(0.0f, 0.0f) }); // top left
				indicies.push_back(vertices.size() - 1);

				vertices.push_back(Vertex{ topLeft + glm::vec3(individualSquareSize, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) }); // top right
				indicies.push_back(vertices.size() - 1);

				vertices.push_back(Vertex{ topLeft + glm::vec3(individualSquareSize, individualSquareSize, 0.0f), glm::vec2(1.0f, 1.0f) }); // bottom right
				indicies.push_back(vertices.size() - 1);				
				
				// tri 2
				vertices.push_back(Vertex{ topLeft + glm::vec3(individualSquareSize, individualSquareSize, 0.0f), glm::vec2(1.0f, 1.0f) }); // bottom right
				indicies.push_back(vertices.size() - 1);				
				
				vertices.push_back(Vertex{ topLeft + glm::vec3(0.0f, individualSquareSize, 0.0f), glm::vec2(0.0f, 1.0f) }); // bottom left
				indicies.push_back(vertices.size() - 1);				
				
				vertices.push_back(Vertex{ topLeft, glm::vec2(0.0f, 0.0f) }); // top left
				indicies.push_back(vertices.size() - 1);
			}
		}

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

	~Plane()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	void Draw()
	{
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

private:
};