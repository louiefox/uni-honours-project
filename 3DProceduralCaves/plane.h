#pragma once

#include <glad/glad.h>
#include <vector>

#include "mesh.h"

class Plane
{
public:
	Plane(unsigned int squaresCount)
	{
		mMesh = Mesh();

		unsigned int planeSquaresWidth = std::sqrt(squaresCount);
		float individualSquareSize = 1.0f / (float)planeSquaresWidth;
		for (int y = 0; y < planeSquaresWidth; y++)
		{
			for (int x = 0; x < planeSquaresWidth; x++)
			{
				glm::vec3 topLeft = glm::vec3(-0.5f + (x * individualSquareSize), -0.5f + (y * individualSquareSize), 0.0f);

				// TODO: Don't repeat vertices, also don't push indicies like this
				// tri 1
				mMesh.addVertex(topLeft, glm::vec2(0.0f, 0.0f)); // top left
				mMesh.addVertex(topLeft + glm::vec3(individualSquareSize, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)); // top right
				mMesh.addVertex(topLeft + glm::vec3(individualSquareSize, individualSquareSize, 0.0f), glm::vec2(1.0f, 1.0f)); // bottom right			
				
				// tri 2
				mMesh.addVertex(topLeft + glm::vec3(individualSquareSize, individualSquareSize, 0.0f), glm::vec2(1.0f, 1.0f)); // bottom right			
				mMesh.addVertex(topLeft + glm::vec3(0.0f, individualSquareSize, 0.0f), glm::vec2(0.0f, 1.0f)); // bottom left			
				mMesh.addVertex(topLeft, glm::vec2(0.0f, 0.0f)); // top left
			}
		}

		mMesh.generate();
	}

	~Plane()
	{

	}

	void draw()
	{
		mMesh.draw();
	}

private:
	Mesh mMesh;
};