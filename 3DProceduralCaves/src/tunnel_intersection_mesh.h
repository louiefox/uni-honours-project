#pragma once

#include <array>
#include <glm/gtc/matrix_transform.hpp>

#include "tunnel_mesh.h"

class TunnelIntersectionMesh : public TunnelMesh
{
public:
	TunnelIntersectionMesh(const float angle);
	~TunnelIntersectionMesh();

private:
	const glm::vec2 getLineEndPos(const glm::vec2& startPos, const float radians, const float lineLength);
	
	void createFloor(const float Y_POS, const float angle);
	void createWall(const glm::vec3 topLeft, const glm::vec3 topRight, const glm::vec3 bottomLeft, const glm::vec3 bottomRight);
	void pushVertexToVector(std::vector<Vertex>& vec, glm::vec3 pos, glm::vec2 texCoords) { vec.push_back(Vertex{pos, texCoords}); } // TEMP
};