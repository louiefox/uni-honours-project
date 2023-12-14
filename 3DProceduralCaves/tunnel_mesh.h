#pragma once

#include <algorithm>
#include <vector>
#include <set>
	
#include "mesh.h"
#include "game_object.h"
#include "PerlinNoise.hpp"

struct comparableVec3
{
	float x, y, z;
};

inline bool operator<(const comparableVec3& lhs, const comparableVec3& rhs)
{
	return std::make_tuple(lhs.x, lhs.y, lhs.z) < std::make_tuple(rhs.x, rhs.y, rhs.z);
}

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

		// Geometry blurring
		splitMeshTriangles(2);
	}	
	
	~TunnelMesh() { }

	void generateGeometryBlurring()
	{
		applyGeometryBlurring();
	}			
	
	void pushGeometryBlurring()
	{
		pushBlurredVertices();
	}			
	
	void generatePerlinNoise()
	{
		applyPerlinNoise();
	}		
	
	void generate()
	{
		mMesh.generate();
	}		
	
	void draw()
	{
		mMesh.draw();
	}	

	const Mesh& getMesh() const
	{
		return mMesh;
	}
	
	void setPreviousTunnelMesh(TunnelMesh* tunnelMesh)
	{
		mPreviousTunnelMesh = tunnelMesh;
	}
	
	void setNextTunnelMesh(TunnelMesh* tunnelMesh)
	{
		mNextTunnelMesh = tunnelMesh;
	}	

protected:
	Mesh mMesh;
	TunnelMesh* mPreviousTunnelMesh = nullptr;
	TunnelMesh* mNextTunnelMesh = nullptr;

	std::vector<Vertex> mTempBlurredVertices;

	void createQuad(const glm::vec3 topLeft, const glm::vec3 topRight, const glm::vec3 bottomLeft, const glm::vec3 bottomRight)
	{
		// Add default vertices for triangles
		mMesh.addVertex(topLeft, glm::vec2(0.0, 0.0)); // top left
		mMesh.addVertex(topRight, glm::vec2(1.0, 0.0)); // top right
		mMesh.addVertex(bottomRight, glm::vec2(1.0, 1.0)); // bottom right

		mMesh.addVertex(topLeft, glm::vec2(0.0, 0.0)); // top left
		mMesh.addVertex(bottomRight, glm::vec2(1.0, 1.0)); // bottom right
		mMesh.addVertex(bottomLeft, glm::vec2(0.0, 1.0)); // bottom left	
	}

	void splitMeshTriangles(int timesToSplit)
	{
		std::vector<Vertex> currentVertices = mMesh.getVertices();
		for (int split = 0; split < timesToSplit; split++)
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

		mMesh.setVertices(currentVertices);
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

	void addVerticesToVector(std::vector<Vertex>& vector, const std::vector<Vertex>& vertices, glm::vec3 worldPosition)
	{
		for (const Vertex& vertex : vertices)
		{
			vector.push_back({ worldPosition + vertex.Position, vertex.TextureCoords });
		}
	}

	void applyGeometryBlurring()
	{
		// TODO: Apply rotation to coordinates, otherwise they aren't correct on the branches
		const glm::vec3& worldPosition = GetPosition();

		// vertices to loop through and modify
		std::vector<Vertex> currentVerticesInWorld;
		addVerticesToVector(currentVerticesInWorld, mMesh.getVertices(), worldPosition);

		// vertices to look for neighbours in - includes previous/next meshes
		std::vector<Vertex> searchVertices;
		addVerticesToVector(searchVertices, mMesh.getVertices(), worldPosition);

		if (mPreviousTunnelMesh != nullptr)
		{
			const std::vector<Vertex>& meshVertices = mPreviousTunnelMesh->getMesh().getVertices();
			addVerticesToVector(searchVertices, meshVertices, mPreviousTunnelMesh->GetPosition());
		}		
		
		if (mNextTunnelMesh != nullptr)
		{
			const std::vector<Vertex>& meshVertices = mNextTunnelMesh->getMesh().getVertices();
			addVerticesToVector(searchVertices, meshVertices, mNextTunnelMesh->GetPosition());
		}

		// loop through current vertices and adjust by neighbours
		mTempBlurredVertices.clear();

		for (const Vertex& vertex : currentVerticesInWorld)
		{
			std::vector<std::tuple<int, float>> nearestVertices;
			std::set<comparableVec3> addedVertices;
			for (int i = 0; i < searchVertices.size(); i++)
			{
				const Vertex& otherVertex = searchVertices[i];
				comparableVec3 compareVec3 = { otherVertex.Position.x, otherVertex.Position.y, otherVertex.Position.z };

				// don't add vertex if same position
				if (otherVertex.Position == vertex.Position)
					continue;				
				
				// don't add vertex if already added
				if (addedVertices.find(compareVec3) != addedVertices.end())
				{
					//std::cout << "found same vector" << std::endl;
					continue;
				}

				addedVertices.insert(compareVec3);

				float distance = glm::distance(vertex.Position, otherVertex.Position);
				nearestVertices.push_back(std::make_tuple(i, distance));
			}

			std::sort(nearestVertices.begin(), nearestVertices.end(), [](std::tuple<int, float>& a, std::tuple<int, float>& b) {
				return std::get<1>(a) < std::get<1>(b); 
			});
			
			// TODO: Use distance with min/max to make further away vertices matter less
			glm::vec3 differenceVec = glm::vec3(0.0, 0.0, 0.0);
			for (int i = 0; i < 4 && i < nearestVertices.size(); i++)
			{
				const Vertex& nearVertex = searchVertices[std::get<0>(nearestVertices[i])];
				differenceVec += nearVertex.Position - vertex.Position;
			}

			mTempBlurredVertices.push_back({ (vertex.Position - worldPosition) + (differenceVec * 0.2f), vertex.TextureCoords });
		}
	}

	void pushBlurredVertices()
	{
		mMesh.setVertices(mTempBlurredVertices);
	}

	void applyPerlinNoise()
	{
		splitMeshTriangles(2);

		const siv::PerlinNoise::seed_type seed = 123456u;
		const siv::PerlinNoise perlin{ seed };

		const std::vector<Vertex>& currentVertices = mMesh.getVertices();

		std::vector<Vertex> newVertices;
		for (int i = 0; i < currentVertices.size(); i++)
		{
			glm::vec3 vertexPosition = currentVertices[i].Position;

			// Apply perlin if not edge vertex
			if (vertexPosition.x > -0.5 && vertexPosition.x < 0.5)
			{
				const double noise = perlin.noise2D_01((GetPosition().x + vertexPosition.x) * 5.0, (GetPosition().z + vertexPosition.z) * 5.0);
				vertexPosition.y = vertexPosition.y - noise * 0.2;
			}

			newVertices.push_back({ vertexPosition, currentVertices[i].TextureCoords });
		}

		mMesh.setVertices(newVertices);
	}
};