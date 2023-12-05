#pragma once

#include <glad/glad.h>
#include <vector>

#include "mesh.h"

class Plane
{
public:
	Plane(unsigned int squaresCount)
	{
		mesh = Mesh();

		unsigned int planeSquaresWidth = std::sqrt(squaresCount);
		float individualSquareSize = 1.0f / (float)planeSquaresWidth;
		for (int y = 0; y < planeSquaresWidth; y++)
		{
			for (int x = 0; x < planeSquaresWidth; x++)
			{
				glm::vec3 topLeft = glm::vec3(-0.5f + (x * individualSquareSize), -0.5f + (y * individualSquareSize), 0.0f);

				// TODO: Don't repeat vertices, also don't push indicies like this
				// tri 1
				mesh.AddVertex(topLeft, glm::vec2(0.0f, 0.0f)); // top left
				mesh.AddVertex(topLeft + glm::vec3(individualSquareSize, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)); // top right
				mesh.AddVertex(topLeft + glm::vec3(individualSquareSize, individualSquareSize, 0.0f), glm::vec2(1.0f, 1.0f)); // bottom right			
				
				// tri 2
				mesh.AddVertex(topLeft + glm::vec3(individualSquareSize, individualSquareSize, 0.0f), glm::vec2(1.0f, 1.0f)); // bottom right			
				mesh.AddVertex(topLeft + glm::vec3(0.0f, individualSquareSize, 0.0f), glm::vec2(0.0f, 1.0f)); // bottom left			
				mesh.AddVertex(topLeft, glm::vec2(0.0f, 0.0f)); // top left
			}
		}

		mesh.Generate();
	}

	~Plane()
	{

	}

	void Draw()
	{
		mesh.Draw();
	}

private:
	Mesh mesh;
};