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
	Mesh()
	{

	}

	~Mesh()
	{
		glDeleteVertexArrays(1, &mVAO);
		glDeleteBuffers(1, &mVBO);
		glDeleteBuffers(1, &mEBO);
	}

	const std::vector<Vertex>& getVertices()
	{
		return mVertices;
	}	
	
	void setVertices(const std::vector<Vertex>& newVertices)
	{
		mVertices = newVertices;
	}

	void addVertex(const glm::vec3& position, const glm::vec2 uvCoords)
	{
		mVertices.push_back(Vertex{ position, uvCoords });
		mIndicies.push_back(mVertices.size() - 1);
	}

	void generate()
	{
		if (mGenerated)
			return;

		mGenerated = true;

		// Create objects
		glGenVertexArrays(1, &mVAO);
		glGenBuffers(1, &mVBO);
		glGenBuffers(1, &mEBO);

		glBindVertexArray(mVAO); // bind VAO before vertex buffers and attributes

		glBindBuffer(GL_ARRAY_BUFFER, mVBO); // bind vertex buffer

		// pass address for first Vertex struct in vertices, they're sequential so the stride/offsets will function normally
		glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW); // GL_STATIC_DRAW means data will not change, GL_DYNAMIC_DRAW would store the vertices in faster memory so the data can be changed frequenetly

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndicies.size() * sizeof(unsigned int), &mIndicies[0], GL_STATIC_DRAW);

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

	void draw()
	{
		glBindVertexArray(mVAO);
		glDrawElements(GL_TRIANGLES, mIndicies.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

private:
	unsigned int mVBO, mVAO, mEBO;
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndicies;

	bool mGenerated = false;
};