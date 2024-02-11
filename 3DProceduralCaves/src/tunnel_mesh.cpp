#include "tunnel_mesh.h"

#include <glm/gtx/norm.hpp>

#include <algorithm>
#include <vector>
#include <set>
#include <iostream>

#include "mesh.h"
#include "game_object.h"
#include "PerlinNoise.hpp"

TunnelMesh::TunnelMesh()
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
	std::cout << mMesh.getVertices().size() << std::endl;
}

TunnelMesh::~TunnelMesh() { }

void TunnelMesh::generateGeometryBlurring(std::vector<TunnelMesh*> tunnelMeshes)
{
	applyGeometryBlurring(tunnelMeshes);
}

void TunnelMesh::pushGeometryBlurring()
{
	pushBlurredVertices();
}

void TunnelMesh::generatePerlinNoise()
{
	applyPerlinNoise();
}

void TunnelMesh::generate()
{
	mMesh.generate();
}

void TunnelMesh::draw()
{
	mMesh.draw();
}

const Mesh& TunnelMesh::getMesh() const
{
	return mMesh;
}

void TunnelMesh::setPreviousTunnelMesh(TunnelMesh* tunnelMesh)
{
	mPreviousTunnelMesh = tunnelMesh;
}

void TunnelMesh::setNextTunnelMesh(TunnelMesh* tunnelMesh)
{
	mNextTunnelMesh = tunnelMesh;
}

void TunnelMesh::setNextTunnelMesh2(TunnelMesh* tunnelMesh)
{
	mNextTunnelMesh2 = tunnelMesh;
}

void TunnelMesh::createQuad(const glm::vec3 topLeft, const glm::vec3 topRight, const glm::vec3 bottomLeft, const glm::vec3 bottomRight)
{
	// Add default vertices for triangles
	mMesh.addVertex(topLeft, glm::vec2(0.0, 0.0)); // top left
	mMesh.addVertex(topRight, glm::vec2(1.0, 0.0)); // top right
	mMesh.addVertex(bottomRight, glm::vec2(1.0, 1.0)); // bottom right

	mMesh.addVertex(topLeft, glm::vec2(0.0, 0.0)); // top left
	mMesh.addVertex(bottomRight, glm::vec2(1.0, 1.0)); // bottom right
	mMesh.addVertex(bottomLeft, glm::vec2(0.0, 1.0)); // bottom left	
}

void TunnelMesh::splitMeshTriangles(int timesToSplit)
{
	std::vector<Vertex> currentVertices = mMesh.getAllVertices();
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

std::vector<Vertex> TunnelMesh::splitTriangle(glm::vec3 vertex1, glm::vec3 vertex2, glm::vec3 vertex3)
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

void TunnelMesh::addVerticesToVector(std::vector<Vertex>& vector, const std::vector<Vertex>& vertices, const glm::vec3& worldPosition, const glm::vec3& worldRotation)
{
	glm::mat4 transformMat = glm::mat4(1.0f);

	transformMat = glm::translate(transformMat, worldPosition);

	transformMat = glm::rotate(transformMat, glm::radians(worldRotation.x), glm::vec3(1.0, 0.0, 0.0));
	transformMat = glm::rotate(transformMat, glm::radians(worldRotation.y), glm::vec3(0.0, 1.0, 0.0));
	transformMat = glm::rotate(transformMat, glm::radians(worldRotation.z), glm::vec3(0.0, 0.0, 1.0));

	for (const Vertex& vertex : vertices)
	{
		glm::vec4 vertexWorldPosition = transformMat * glm::vec4(vertex.Position, 1.0f);
		vector.push_back({ glm::vec3(vertexWorldPosition), vertex.TextureCoords});
	}
}

void TunnelMesh::applyGeometryBlurring(std::vector<TunnelMesh*> tunnelMeshes)
{
	const glm::vec3& worldPosition = GetPosition();
	const glm::vec3& worldRotation = GetRotation();

	// vertices to loop through and modify
	std::vector<Vertex> currentVerticesInWorld;
	addVerticesToVector(currentVerticesInWorld, mMesh.getAllVertices(), worldPosition, worldRotation);

	// vertices to look for neighbours in - includes previous/next meshes
	std::vector<Vertex> searchVertices;
	addVerticesToVector(searchVertices, mMesh.getAllVertices(), worldPosition, worldRotation);

	//if (mPreviousTunnelMesh != nullptr)
	//{
	//	const std::vector<Vertex>& meshVertices = mPreviousTunnelMesh->getMesh().getAllVertices();
	//	addVerticesToVector(searchVertices, meshVertices, mPreviousTunnelMesh->GetPosition(), mPreviousTunnelMesh->GetRotation());
	//}

	//if (mNextTunnelMesh != nullptr)
	//{
	//	const std::vector<Vertex>& meshVertices = mNextTunnelMesh->getMesh().getAllVertices();
	//	addVerticesToVector(searchVertices, meshVertices, mNextTunnelMesh->GetPosition(), mNextTunnelMesh->GetRotation());
	//}	
	//
	//if (mNextTunnelMesh2 != nullptr)
	//{
	//	const std::vector<Vertex>& meshVertices = mNextTunnelMesh2->getMesh().getAllVertices();
	//	addVerticesToVector(searchVertices, meshVertices, mNextTunnelMesh2->GetPosition(), mNextTunnelMesh2->GetRotation());
	//}

	// TEMP: Until adjacent meshes is fixed
	for (TunnelMesh* mesh : tunnelMeshes)
	{
		const std::vector<Vertex>& meshVertices = mesh->getMesh().getAllVertices();
		addVerticesToVector(searchVertices, meshVertices, mesh->GetPosition(), mesh->GetRotation());
	}

	// loop through current vertices and adjust by neighbours
	mTempBlurredVertices.clear();

	glm::mat4 inverseTransformMat = glm::mat4(1.0f);

	inverseTransformMat = glm::rotate(inverseTransformMat, glm::radians(-worldRotation.x), glm::vec3(1.0, 0.0, 0.0));
	inverseTransformMat = glm::rotate(inverseTransformMat, glm::radians(-worldRotation.y), glm::vec3(0.0, 1.0, 0.0));
	inverseTransformMat = glm::rotate(inverseTransformMat, glm::radians(-worldRotation.z), glm::vec3(0.0, 0.0, 1.0));

	inverseTransformMat = glm::translate(inverseTransformMat, -worldPosition);

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

			float distance = glm::length2(otherVertex.Position - vertex.Position); // glm::distance(vertex.Position, otherVertex.Position);
			nearestVertices.push_back(std::make_tuple(i, distance));
		}

		std::sort(nearestVertices.begin(), nearestVertices.end(), [](std::tuple<int, float>& a, std::tuple<int, float>& b) {
			return std::get<1>(a) < std::get<1>(b);
			});

		// TODO: Use distance with min/max to make further away vertices matter less
		const int neighboursToCompare = 4;

		float minDistance = std::numeric_limits<float>::max(), maxDistance = 0.0f;
		for (int i = 0; i < neighboursToCompare && i < nearestVertices.size(); i++)
		{
			const float distance = std::get<1>(nearestVertices[i]);
			minDistance = std::fmin(minDistance, distance);
			maxDistance = std::fmax(maxDistance, distance);
		}

		glm::vec3 differenceVec = glm::vec3(0.0, 0.0, 0.0);
		for (int i = 0; i < neighboursToCompare && i < nearestVertices.size(); i++)
		{
			const float distance = std::get<1>(nearestVertices[i]);
			const float minMaxScale = minDistance == maxDistance ? 1.0f : 1.0f - ((distance - minDistance) / (maxDistance - minDistance)); // if min/max distance are the same don't scale

			const Vertex& nearVertex = searchVertices[std::get<0>(nearestVertices[i])];
			differenceVec += (nearVertex.Position - vertex.Position) * minMaxScale;
		}

		//mTempBlurredVertices.push_back({ ((vertex.Position - worldPosition) / worldRotation) + (differenceVec * 0.2f), vertex.TextureCoords });

		glm::vec4 vertexLocalPosition = inverseTransformMat * glm::vec4(vertex.Position + (differenceVec * 0.2f), 1.0f);
		mTempBlurredVertices.push_back({ glm::vec3(vertexLocalPosition), vertex.TextureCoords});
	}

	//glm::mat4 inverseTransformMat = glm::mat4(1.0f);

	//inverseTransformMat = glm::rotate(inverseTransformMat, glm::radians(-worldRotation.x), glm::vec3(1.0, 0.0, 0.0));
	//inverseTransformMat = glm::rotate(inverseTransformMat, glm::radians(-worldRotation.y), glm::vec3(0.0, 1.0, 0.0));
	//inverseTransformMat = glm::rotate(inverseTransformMat, glm::radians(-worldRotation.z), glm::vec3(0.0, 0.0, 1.0));

	//inverseTransformMat = glm::translate(inverseTransformMat, -worldPosition);

	//for (int i = 0; i < mTempBlurredVertices.size(); i++)
	//{
	//	const Vertex& tempVertex = mTempBlurredVertices[i];

	//	glm::vec4 vertexLocalPosition = inverseTransformMat * glm::vec4(tempVertex.Position, 1.0f);
	//	mTempBlurredVertices[i] = { glm::vec3(vertexLocalPosition), tempVertex.TextureCoords };
	//}
}

void TunnelMesh::pushBlurredVertices()
{
	mMesh.setVertices(mTempBlurredVertices);
}

void TunnelMesh::applyPerlinNoise()
{
	splitMeshTriangles(2);

	const siv::PerlinNoise::seed_type seed = 123456u;
	const siv::PerlinNoise perlin{ seed };

	const std::vector<Vertex>& currentVertices = mMesh.getAllVertices();

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