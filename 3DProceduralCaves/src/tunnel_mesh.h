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

class TunnelMesh : public GameObject
{
public:
	TunnelMesh();
	~TunnelMesh();

	void generateGeometryBlurring();
	void pushGeometryBlurring();
	void generatePerlinNoise();
	
	void generate();
	void draw();

	const Mesh& getMesh() const;
	
	void setPreviousTunnelMesh(TunnelMesh* tunnelMesh);
	void setNextTunnelMesh(TunnelMesh* tunnelMesh);
	void setNextTunnelMesh2(TunnelMesh* tunnelMesh);	
	
	const TunnelMesh* getPreviousTunnelMesh() { return mPreviousTunnelMesh; }
	const TunnelMesh* getNextTunnelMesh() { return mNextTunnelMesh; }
	const TunnelMesh* getNextTunnelMesh2() { return mNextTunnelMesh2; }

protected:
	Mesh mMesh;
	TunnelMesh* mPreviousTunnelMesh = nullptr;
	TunnelMesh* mNextTunnelMesh = nullptr;
	TunnelMesh* mNextTunnelMesh2 = nullptr;

	std::vector<Vertex> mTempBlurredVertices;

	void createQuad(const glm::vec3 topLeft, const glm::vec3 topRight, const glm::vec3 bottomLeft, const glm::vec3 bottomRight);
	void splitMeshTriangles(int timesToSplit);
	std::vector<Vertex> splitTriangle(glm::vec3 vertex1, glm::vec3 vertex2, glm::vec3 vertex3);
	void addVerticesToVector(std::vector<Vertex>& vector, const std::vector<Vertex>& vertices, const glm::vec3& worldPosition, const glm::vec3& worldRotation);

	void applyGeometryBlurring();
	void pushBlurredVertices();
	void applyPerlinNoise();
};