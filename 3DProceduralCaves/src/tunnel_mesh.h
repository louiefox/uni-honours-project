#pragma once

#include <glm/gtx/norm.hpp>

#include <algorithm>
#include <vector>
#include <set>
#include <iostream>
	
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

struct Vec3KeyFuncs
{
	std::size_t operator()(const glm::vec3& lhs) const
	{
		return std::hash<float>()(lhs.x) ^ (std::hash<float>()(lhs.y) << 1) ^ (std::hash<float>()(lhs.z) << 2);
	}

	bool operator()(const glm::vec3& lhs, const glm::vec3& rhs) const
	{
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
	}
};

class TunnelMesh : public GameObject
{
public:
	TunnelMesh();
	~TunnelMesh();

	void generateGeometryBlurring(std::vector<TunnelMesh*> tunnelMeshes);
	void pushGeometryBlurring();
	void generatePerlinNoise();
	void splitMeshTriangles(int timesToSplit);
	void calculateNormals() { mMesh.calculateNormals(); }
	void createNormalLines() { mMesh.createNormalLines(); }
	
	void generate();
	void draw();
	void drawNormalLines();

	const Mesh& getMesh() const { return mMesh; }
	Mesh& getMesh() { return mMesh; }
	
	void setPreviousTunnelMesh(TunnelMesh* tunnelMesh);
	void setNextTunnelMesh(TunnelMesh* tunnelMesh);
	void setNextTunnelMesh2(TunnelMesh* tunnelMesh);	
	
	const TunnelMesh* getPreviousTunnelMesh() { return mPreviousTunnelMesh; }
	const TunnelMesh* getNextTunnelMesh() { return mNextTunnelMesh; }
	const TunnelMesh* getNextTunnelMesh2() { return mNextTunnelMesh2; }

	glm::mat4 getWorldMatrix();
	glm::mat4 getWorldMatrix(const glm::vec3& worldPosition, const glm::vec3& worldRotation);
	glm::mat4 getWorldInverseMatrix();
	glm::vec3 transformVecByMatrix(const glm::vec3& coords, const glm::mat4& matrix);

protected:
	Mesh mMesh;
	TunnelMesh* mPreviousTunnelMesh = nullptr;
	TunnelMesh* mNextTunnelMesh = nullptr;
	TunnelMesh* mNextTunnelMesh2 = nullptr;

	std::vector<Vertex> mTempBlurredVertices;

	void createQuad(const glm::vec3 topLeft, const glm::vec3 topRight, const glm::vec3 bottomLeft, const glm::vec3 bottomRight);
	std::vector<Vertex> splitTriangle(const Vertex& vertex1, const Vertex& vertex2, const Vertex& vertex3);
	void addVerticesToVector(std::vector<Vertex>& vector, const std::vector<Vertex>& vertices, const glm::vec3& worldPosition, const glm::vec3& worldRotation);

	void applyGeometryBlurring(std::vector<TunnelMesh*> tunnelMeshes);
	void pushBlurredVertices();
	void applyPerlinNoise();
};