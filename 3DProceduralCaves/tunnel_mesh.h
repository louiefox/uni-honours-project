#pragma once

#include <algorithm>
#include <vector>
#include <map>
	
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

		// Blur geometry
		applyGeometryBlurring();

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
		for (int split = 0; split < 3; split++)
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

	void applyGeometryBlurring()
	{
		const std::vector<Vertex>& currentVertices = mMesh.getVertices();

		std::vector<Vertex> newVertices;
		for (const Vertex& vertex : currentVertices)
		{
			std::vector<std::tuple<int, float>> nearestVertices;
			std::map<float, bool> addedVertices;
			for (int i = 0; i < currentVertices.size(); i++)
			{
				const Vertex& otherVertex = currentVertices[i];

				float distance = glm::distance(vertex.Position, otherVertex.Position);

				// don't add vertex if same position or already added as nearby
				if (otherVertex.Position == vertex.Position || addedVertices.contains(distance))
					continue;

				addedVertices[distance] = true;

				nearestVertices.push_back(std::make_tuple(i, distance));
			}

			std::sort(nearestVertices.begin(), nearestVertices.end(), [](std::tuple<int, float>& a, std::tuple<int, float>& b) {
				return std::get<1>(a) < std::get<1>(b); 
			});
			
			// TODO: Use distance with min/max to make further away vertices matter less
			glm::vec3 differenceVec = glm::vec3(0.0, 0.0, 0.0);
			for (int i = 0; i < 4 && i < nearestVertices.size(); i++)
			{
				const Vertex& nearVertex = currentVertices[std::get<0>(nearestVertices[i])];
				differenceVec += nearVertex.Position - vertex.Position;
			}

			newVertices.push_back({ vertex.Position + (differenceVec * 0.1f), vertex.TextureCoords });
		}

		mMesh.setVertices(newVertices);
	}
};