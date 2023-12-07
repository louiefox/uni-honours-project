#pragma once

#include "mesh.h"
#include "game_object.h"
#include "PerlinNoise.hpp"

class TunnelMesh : public GameObject
{
public:
	TunnelMesh() 
	{ 
		mMesh = Mesh();

		// floor
		createQuad(
			glm::vec3(-0.5, -0.5, -0.5),
			glm::vec3(0.5, -0.5, -0.5),
			glm::vec3(-0.5, -0.5, 0.5),
			glm::vec3(0.5, -0.5, 0.5),
			false
		);		
		
		// ceiling
		createQuad(
			glm::vec3(-0.5, 0.5, -0.5),
			glm::vec3(0.5, 0.5, -0.5),
			glm::vec3(-0.5, 0.5, 0.5),
			glm::vec3(0.5, 0.5, 0.5),
			true
		);		
		
		// left wall
		createQuad(
			glm::vec3(-0.5, 0.5, 0.5),
			glm::vec3(-0.5, 0.5, -0.5),
			glm::vec3(-0.5, -0.5, 0.5),
			glm::vec3(-0.5, -0.5, -0.5),
			true
		);		
		
		// right wall
		createQuad(
			glm::vec3(0.5, 0.5, 0.5),
			glm::vec3(0.5, 0.5, -0.5),
			glm::vec3(0.5, -0.5, 0.5),
			glm::vec3(0.5, -0.5, -0.5),
			true
		);

		mMesh.generate();
	}	
	
	~TunnelMesh()
	{ 

	}

	void draw()
	{
		mMesh.draw();
	}

private:
	Mesh mMesh;

	void createQuad(const glm::vec3 topLeft, const glm::vec3 topRight, const glm::vec3 bottomLeft, const glm::vec3 bottomRight, bool useOldCode)
	{
		if (useOldCode)
		{
			mMesh.addVertex(topLeft, glm::vec2(0.0, 0.0)); // top left
			mMesh.addVertex(topRight, glm::vec2(1.0, 0.0)); // top right
			mMesh.addVertex(bottomRight, glm::vec2(1.0, 1.0)); // bottom right

			mMesh.addVertex(topLeft, glm::vec2(0.0, 0.0)); // top left
			mMesh.addVertex(bottomRight, glm::vec2(1.0, 1.0)); // bottom right
			mMesh.addVertex(bottomLeft, glm::vec2(0.0, 1.0)); // bottom left	

			return;
		}

		// Add default vertices for triangles
		std::vector<Vertex> currentVertices;
		currentVertices.push_back({ topLeft, glm::vec2(0.0, 0.0) }); // top left
		currentVertices.push_back({ topRight, glm::vec2(1.0, 0.0) }); // top right
		currentVertices.push_back({ bottomRight, glm::vec2(1.0, 1.0) }); // bottom right

		currentVertices.push_back({ topLeft, glm::vec2(0.0, 0.0) }); // top left
		currentVertices.push_back({ bottomRight, glm::vec2(1.0, 1.0) }); // bottom right
		currentVertices.push_back({ bottomLeft, glm::vec2(0.0, 1.0) }); // bottom left	

		// Start splitting
		for (int split = 0; split < 5; split++)
		{
			// Loop through existing triangles
			std::vector<Vertex> newVertices;
			for (int tri = 0; tri < currentVertices.size(); tri += 3)
			{
				// Split triangle and add to new vertices vector
				std::vector<Vertex> splitVertices = splitTriangle(currentVertices[tri].Position, currentVertices[tri + 1].Position, currentVertices[tri + 2].Position);
				for (int i = 0; i < splitVertices.size(); i++)
				{
					newVertices.push_back(splitVertices[i]);
				}
			}

			// Swap out vertices
			currentVertices = newVertices;
		}

		// Add new triangles to mesh

		const siv::PerlinNoise::seed_type seed = 123456u;
		const siv::PerlinNoise perlin{ seed };

		for (int i = 0; i < currentVertices.size(); i++)
		{
			glm::vec3 vertexPosition = currentVertices[i].Position;

			// Apply perlin if not edge vertex
			if (vertexPosition.x > -0.5 && vertexPosition.x < 0.5 && vertexPosition.z > -0.5 && vertexPosition.z < 0.5)
			{
				const double noise = perlin.noise2D_01(vertexPosition.x * 5.0, vertexPosition.z * 5.0);
				vertexPosition.y = vertexPosition.y - noise * 0.2;
			}

			mMesh.addVertex(vertexPosition, currentVertices[i].TextureCoords);
		}
	}

	std::vector<Vertex> splitTriangle(glm::vec3 vertex1, glm::vec3 vertex2, glm::vec3 vertex3)
	{
		const float yPos = vertex1.y; // TEMP

		std::vector<Vertex> newVertices;

		glm::vec2 midPoint1 = getLineMidPoint(glm::vec2(vertex1.x, vertex1.z), glm::vec2(vertex2.x, vertex2.z));
		glm::vec2 midPoint2 = getLineMidPoint(glm::vec2(vertex2.x, vertex2.z), glm::vec2(vertex3.x, vertex3.z));
		glm::vec2 midPoint3 = getLineMidPoint(glm::vec2(vertex3.x, vertex3.z), glm::vec2(vertex1.x, vertex1.z));

		// center
		newVertices.push_back({ glm::vec3(midPoint1.x, yPos, midPoint1.y), glm::vec2(0.5, 0.0) });
		newVertices.push_back({ glm::vec3(midPoint2.x, yPos, midPoint2.y), glm::vec2(1.0, 0.5) });
		newVertices.push_back({ glm::vec3(midPoint3.x, yPos, midPoint3.y), glm::vec2(0.5, 0.5) });

		// vertex1
		newVertices.push_back({ glm::vec3(vertex1.x, yPos, vertex1.z), glm::vec2(0.0, 0.0) });
		newVertices.push_back({ glm::vec3(midPoint1.x, yPos, midPoint1.y), glm::vec2(0.5, 0.0) });
		newVertices.push_back({ glm::vec3(midPoint3.x, yPos, midPoint3.y), glm::vec2(0.5, 0.5) });

		// vertex2
		newVertices.push_back({ glm::vec3(midPoint1.x, yPos, midPoint1.y), glm::vec2(0.5, 0.0) });
		newVertices.push_back({ glm::vec3(vertex2.x, yPos, vertex2.z), glm::vec2(1.0, 0.0) });
		newVertices.push_back({ glm::vec3(midPoint2.x, yPos, midPoint2.y), glm::vec2(1.0, 0.5) });

		// vertex3
		newVertices.push_back({ glm::vec3(midPoint3.x, yPos, midPoint3.y), glm::vec2(0.5, 0.5) });
		newVertices.push_back({ glm::vec3(midPoint2.x, yPos, midPoint2.y), glm::vec2(1.0, 0.5) });
		newVertices.push_back({ glm::vec3(vertex3.x, yPos, vertex3.z), glm::vec2(1.0, 1.0) });

		return newVertices;
	}

	glm::vec2 getLineMidPoint(const glm::vec2& start, const glm::vec2& end)
	{
		return start + ((end - start) / 2.0f);
	}
};