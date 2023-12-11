#pragma once

#include "mesh.h"
#include "game_object.h"
#include "PerlinNoise.hpp"

class TunnelMesh : public GameObject
{
public:
	TunnelMesh(bool tempDevEnablePerlin)
	{ 
		mMesh = Mesh();

		// floor
		createQuad(
			glm::vec3(-0.5, -0.5, -0.5),
			glm::vec3(0.5, -0.5, -0.5),
			glm::vec3(-0.5, -0.5, 0.5),
			glm::vec3(0.5, -0.5, 0.5)
		);		
		
		// ceiling
		createQuad(
			glm::vec3(-0.5, 0.5, -0.5),
			glm::vec3(0.5, 0.5, -0.5),
			glm::vec3(-0.5, 0.5, 0.5),
			glm::vec3(0.5, 0.5, 0.5)
		);		
		
		// left wall
		createQuad(
			glm::vec3(-0.5, 0.5, 0.5),
			glm::vec3(-0.5, 0.5, -0.5),
			glm::vec3(-0.5, -0.5, 0.5),
			glm::vec3(-0.5, -0.5, -0.5)
		);		
		
		// right wall
		createQuad(
			glm::vec3(0.5, 0.5, 0.5),
			glm::vec3(0.5, 0.5, -0.5),
			glm::vec3(0.5, -0.5, 0.5),
			glm::vec3(0.5, -0.5, -0.5)
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

	void createQuad(const glm::vec3 topLeft, const glm::vec3 topRight, const glm::vec3 bottomLeft, const glm::vec3 bottomRight)
	{
		if (false)
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
		for (int split = 0; split < 4; split++)
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
		for (int i = 0; i < currentVertices.size(); i++)
		{
			mMesh.addVertex(currentVertices[i].Position, currentVertices[i].TextureCoords);
		}

		//const siv::PerlinNoise::seed_type seed = 123456u;
		//const siv::PerlinNoise perlin{ seed };

		//for (int i = 0; i < currentVertices.size(); i++)
		//{
		//	glm::vec3 vertexPosition = currentVertices[i].Position;

		//	// Apply perlin if not edge vertex
		//	if (vertexPosition.x > -0.5 && vertexPosition.x < 0.5 && vertexPosition.z > -0.5 && vertexPosition.z < 0.5)
		//	{
		//		const double noise = perlin.noise2D_01(vertexPosition.x * 5.0, vertexPosition.z * 5.0);
		//		vertexPosition.y = vertexPosition.y - noise * 0.2;
		//	}

		//	mMesh.addVertex(vertexPosition, currentVertices[i].TextureCoords);
		//}
	}

	std::vector<Vertex> splitTriangle(glm::vec3 vertex1, glm::vec3 vertex2, glm::vec3 vertex3)
	{
		std::vector<Vertex> newVertices;

		glm::vec3 midPoint1 = vertex1 + ((vertex2 - vertex1) / 2.0f);
		glm::vec3 midPoint2 = vertex2 + ((vertex3 - vertex2) / 2.0f);
		glm::vec3 midPoint3 = vertex3 + ((vertex1 - vertex3) / 2.0f);

		// center
		newVertices.push_back({ midPoint1, glm::vec2(0.5, 0.0) });
		newVertices.push_back({ midPoint2, glm::vec2(1.0, 0.5) });
		newVertices.push_back({ midPoint3, glm::vec2(0.5, 0.5) });

		// vertex1
		newVertices.push_back({ vertex1, glm::vec2(0.0, 0.0) });
		newVertices.push_back({ midPoint1, glm::vec2(0.5, 0.0) });
		newVertices.push_back({ midPoint3, glm::vec2(0.5, 0.5) });

		// vertex2
		newVertices.push_back({ midPoint1, glm::vec2(0.5, 0.0) });
		newVertices.push_back({ vertex2, glm::vec2(1.0, 0.0) });
		newVertices.push_back({ midPoint2, glm::vec2(1.0, 0.5) });

		// vertex3
		newVertices.push_back({ midPoint3, glm::vec2(0.5, 0.5) });
		newVertices.push_back({ midPoint2, glm::vec2(1.0, 0.5) });
		newVertices.push_back({ vertex3, glm::vec2(1.0, 1.0) });

		return newVertices;
	}
};