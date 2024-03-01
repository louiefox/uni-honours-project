#include "tunnel_mesh.h"

#include <glm/gtx/norm.hpp>

#include <algorithm>
#include <vector>
#include <set>
#include <iostream>
#include <unordered_set>

#include "mesh.h"
#include "game_object.h"
#include "PerlinNoise.hpp"

TunnelMesh::TunnelMesh()
{
	mMesh = Mesh();

	// floor
	createQuad(
		glm::vec3(0.5, -0.5, -0.5),
		glm::vec3(-0.5, -0.5, -0.5),
		glm::vec3(0.5, -0.5, 0.5),
		glm::vec3(-0.5, -0.5, 0.5)
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
		glm::vec3(-0.5, 0.5, -0.5),
		glm::vec3(-0.5, 0.5, 0.5),
		glm::vec3(-0.5, -0.5, -0.5),
		glm::vec3(-0.5, -0.5, 0.5)
	);

	// right wall
	createQuad(
		glm::vec3(0.5, 0.5, 0.5),
		glm::vec3(0.5, 0.5, -0.5),
		glm::vec3(0.5, -0.5, 0.5),
		glm::vec3(0.5, -0.5, -0.5)
	);
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
	mMesh.calculateNormals();
	mMesh.generate();
}

void TunnelMesh::draw()
{
	mMesh.draw();
}

void TunnelMesh::drawNormalLines()
{
	mMesh.drawNormalLines();
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
	newVertices.push_back({ midPoint1, glm::vec2(0.0, 0.0) });
	newVertices.push_back({ midPoint2, glm::vec2(1.0, 0.0) });
	newVertices.push_back({ midPoint3, glm::vec2(1.0, 1.0) });

	// vertex1
	newVertices.push_back({ vertex1, glm::vec2(0.0, 0.0) });
	newVertices.push_back({ midPoint1, glm::vec2(1.0, 0.0) });
	newVertices.push_back({ midPoint3, glm::vec2(1.0, 1.0) });

	// vertex2
	newVertices.push_back({ midPoint1, glm::vec2(0.0, 0.0) });
	newVertices.push_back({ vertex2, glm::vec2(1.0, 0.0) });
	newVertices.push_back({ midPoint2, glm::vec2(1.0, 1.0) });

	// vertex3
	newVertices.push_back({ midPoint3, glm::vec2(0.0, 0.0) });
	newVertices.push_back({ midPoint2, glm::vec2(1.0, 0.0) });
	newVertices.push_back({ vertex3, glm::vec2(1.0, 1.0) });

	return newVertices;
}

void TunnelMesh::addVerticesToVector(std::vector<Vertex>& vector, const std::vector<Vertex>& vertices, const glm::vec3& worldPosition, const glm::vec3& worldRotation)
{
	const glm::mat4 transformMat = getWorldMatrix(worldPosition, worldRotation);

	for (const Vertex& vertex : vertices)
	{
		vector.push_back({ transformVecByMatrix(vertex.Position, transformMat), vertex.TextureCoords});
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
	std::vector<Vertex> tempSearchVertices;
	addVerticesToVector(tempSearchVertices, mMesh.getAllVertices(), worldPosition, worldRotation);

	//if (mPreviousTunnelMesh != nullptr)
	//{
	//	const std::vector<Vertex>& meshVertices = mPreviousTunnelMesh->getMesh().getAllVertices();
	//	addVerticesToVector(tempSearchVertices, meshVertices, mPreviousTunnelMesh->GetPosition(), mPreviousTunnelMesh->GetRotation());
	//}

	//if (mNextTunnelMesh != nullptr)
	//{
	//	const std::vector<Vertex>& meshVertices = mNextTunnelMesh->getMesh().getAllVertices();
	//	addVerticesToVector(tempSearchVertices, meshVertices, mNextTunnelMesh->GetPosition(), mNextTunnelMesh->GetRotation());
	//}	
	//
	//if (mNextTunnelMesh2 != nullptr)
	//{
	//	const std::vector<Vertex>& meshVertices = mNextTunnelMesh2->getMesh().getAllVertices();
	//	addVerticesToVector(tempSearchVertices, meshVertices, mNextTunnelMesh2->GetPosition(), mNextTunnelMesh2->GetRotation());
	//}

	// TEMP: Until adjacent meshes is fixed
	for (TunnelMesh* mesh : tunnelMeshes)
	{
		const std::vector<Vertex>& meshVertices = mesh->getMesh().getAllVertices();
		addVerticesToVector(tempSearchVertices, meshVertices, mesh->GetPosition(), mesh->GetRotation());
	}

	// loop through current vertices and adjust by neighbours
	mTempBlurredVertices.clear();

	glm::mat4 inverseTransformMat = getWorldInverseMatrix();

	// remove any duplicate vertices in search
	std::set<comparableVec3> uniqueSearchVertices;
	std::vector<Vertex> searchVertices;
	for (int i = 0; i < tempSearchVertices.size(); i++)
	{
		const Vertex& vertex = tempSearchVertices[i];
		comparableVec3 compareVec3 = { vertex.Position.x, vertex.Position.y, vertex.Position.z };

		if (uniqueSearchVertices.find(compareVec3) != uniqueSearchVertices.end())
		{
			continue;
		}

		uniqueSearchVertices.insert(compareVec3);
		searchVertices.push_back(vertex);
	}

	// loop through this mesh's vertices and blur
	for (const Vertex& vertex : currentVerticesInWorld)
	{
		const int neighboursToCompare = 4;

		std::vector<std::tuple<int, float>> nearestVertices;

		for (int i = 0; i < searchVertices.size(); i++)
		{
			const Vertex& otherVertex = searchVertices[i];

			// don't add vertex if same position
			if (otherVertex.Position == vertex.Position)
				continue;

			float distance = glm::length2(otherVertex.Position - vertex.Position); // glm::distance(vertex.Position, otherVertex.Position);

			// check if should add to nearest vertices
			bool addedToNearest = false;
			if (nearestVertices.size() < neighboursToCompare)
			{
				nearestVertices.push_back(std::make_tuple(i, distance));
				addedToNearest = true;
			}
			else if (distance < std::get<1>(nearestVertices[neighboursToCompare - 1])) // check if distance is less than highest stored distance
			{
				nearestVertices[neighboursToCompare - 1] = std::make_tuple(i, distance);
				addedToNearest = true;
			}

			// if added then move as far to front as possible
			if (addedToNearest && nearestVertices.size() > 1)
			{
				for (int i = std::min(neighboursToCompare - 1, (int)nearestVertices.size() - 1); i > 0; i--)
				{
					if (std::get<1>(nearestVertices[i]) < std::get<1>(nearestVertices[i - 1]))
					{
						std::swap(nearestVertices[i], nearestVertices[i - 1]);
					}
					else
					{
						break;
					}
				}
			}
		}

		// adjust based on min/max scalar
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

		mTempBlurredVertices.push_back({ transformVecByMatrix(vertex.Position + (differenceVec * 0.2f), inverseTransformMat), vertex.TextureCoords});
	}
}

void TunnelMesh::pushBlurredVertices()
{
	mMesh.setVertices(mTempBlurredVertices);
}

void TunnelMesh::applyPerlinNoise()
{
	splitMeshTriangles(2);
	mMesh.calculateNormals();

	const siv::PerlinNoise::seed_type seed = 123456u;
	const siv::PerlinNoise perlin{ seed };

	const std::vector<Vertex>& currentVertices = mMesh.getAllVertices();

	const glm::mat4 transformMat = getWorldMatrix();
	const glm::mat4 inverseTransformMat = getWorldInverseMatrix();

	std::vector<Vertex> newVertices;
	for (int i = 0; i < currentVertices.size(); i++)
	{
		glm::vec3 vertexPosition = transformVecByMatrix(currentVertices[i].Position, transformMat);

		//double noise = perlin.noise2D_01(vertexPosition.x * 5.0, vertexPosition.z * 5.0);
		//double noise2 = perlin.noise2D_01(vertexPosition.x * 25.0, vertexPosition.z * 25.0);
		////if (vertexPosition.x > -0.5 && vertexPosition.x < 0.5)
		//{
		//	//noise = perlin.noise2D_01((GetPosition().y + vertexPosition.y) * 5.0, (GetPosition().z + vertexPosition.z) * 5.0);
		//	vertexPosition.y -= noise * 0.1;
		//	vertexPosition.y -= noise2 * 0.05;
		//}

		//vertexPosition -= currentVertices[i].Normal * (float)noise * 0.5f;

		float noise = perlin.noise3D(vertexPosition.x * 3.0, vertexPosition.y * 3.0, vertexPosition.z * 3.0);
		vertexPosition.x += noise * 0.05;
		vertexPosition.y += noise * 0.05;
		vertexPosition.z += noise * 0.05;		
		
		float noise2 = perlin.noise3D(vertexPosition.x * 10.0, vertexPosition.y * 10.0, vertexPosition.z * 10.0);
		vertexPosition.x += noise2 * 0.02;
		vertexPosition.y += noise2 * 0.02;
		vertexPosition.z += noise2 * 0.02;		
		
		float noise3 = perlin.noise3D(vertexPosition.x * 0.5, vertexPosition.y * 0.5, vertexPosition.z * 0.5);
		vertexPosition.x += noise3 * 0.5;
		vertexPosition.y += noise3 * 0.3;
		vertexPosition.z += noise3 * 0.5;

		newVertices.push_back({ transformVecByMatrix(vertexPosition, inverseTransformMat), currentVertices[i].TextureCoords, currentVertices[i].Normal });
	}

	mMesh.setVertices(newVertices);
}

glm::mat4 TunnelMesh::getWorldMatrix(const glm::vec3& worldPosition, const glm::vec3& worldRotation)
{
	glm::mat4 transformMat = glm::mat4(1.0f);

	transformMat = glm::translate(transformMat, worldPosition);

	transformMat = glm::rotate(transformMat, glm::radians(worldRotation.x), glm::vec3(1.0, 0.0, 0.0));
	transformMat = glm::rotate(transformMat, glm::radians(worldRotation.y), glm::vec3(0.0, 1.0, 0.0));
	transformMat = glm::rotate(transformMat, glm::radians(worldRotation.z), glm::vec3(0.0, 0.0, 1.0));

	return transformMat;
}

glm::mat4 TunnelMesh::getWorldMatrix()
{
	return getWorldMatrix(GetPosition(), GetRotation());
}

glm::mat4 TunnelMesh::getWorldInverseMatrix()
{
	glm::mat4 inverseTransformMat = glm::mat4(1.0f);

	const glm::vec3 worldRotation = GetRotation();
	inverseTransformMat = glm::rotate(inverseTransformMat, glm::radians(-worldRotation.x), glm::vec3(1.0, 0.0, 0.0));
	inverseTransformMat = glm::rotate(inverseTransformMat, glm::radians(-worldRotation.y), glm::vec3(0.0, 1.0, 0.0));
	inverseTransformMat = glm::rotate(inverseTransformMat, glm::radians(-worldRotation.z), glm::vec3(0.0, 0.0, 1.0));

	inverseTransformMat = glm::translate(inverseTransformMat, -GetPosition());

	return inverseTransformMat;
}

glm::vec3 TunnelMesh::transformVecByMatrix(const glm::vec3& coords, const glm::mat4& matrix)
{
	return glm::vec3(matrix * glm::vec4(coords, 1.0f));
}