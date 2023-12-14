#pragma once

#include <array>
#include <glm/gtc/matrix_transform.hpp>

#include "tunnel_mesh.h"

class TunnelIntersectionMesh : public TunnelMesh
{
public:
	TunnelIntersectionMesh(const float angle) 
	{
		mMesh = Mesh();

		const glm::vec2 START_POS = glm::vec2(0.0, 0.0);

		// Floors
		createFloor(-0.5f, angle);
		createFloor(0.5f, angle);

		// Walls
		createWall(glm::vec3(-0.5, 0.5, 1.0), glm::vec3(-0.5, 0.5, 0.0), glm::vec3(-0.5, -0.5, 1.0), glm::vec3(-0.5, -0.5, 0.0)); // top left, top right, bottom left, bottom right
		createWall(glm::vec3(0.5, 0.5, 1.0), glm::vec3(0.5, 0.5, 0.0), glm::vec3(0.5, -0.5, 1.0), glm::vec3(0.5, -0.5, 0.0));

		// Left join
		const glm::vec2& LEFT_TURN_END = -getLineEndPos(START_POS, glm::radians(angle), 1.0f);
		const glm::vec2& LEFT_TURN_PERPENDICULAR_L = getLineEndPos(LEFT_TURN_END, glm::radians(angle + 90.0f), 0.5f);

		createWall(glm::vec3(-0.5, 0.5, 0.0), glm::vec3(LEFT_TURN_PERPENDICULAR_L.x, 0.5, LEFT_TURN_PERPENDICULAR_L.y), glm::vec3(-0.5, -0.5, 0.0), glm::vec3(LEFT_TURN_PERPENDICULAR_L.x, -0.5, LEFT_TURN_PERPENDICULAR_L.y));		
		
		// Right join
		glm::vec2 RIGHT_TURN_END = getLineEndPos(START_POS, glm::radians(angle), 1.0f);
		RIGHT_TURN_END.y = -RIGHT_TURN_END.y;
		const glm::vec2& RIGHT_TURN_PERPENDICULAR_R = getLineEndPos(RIGHT_TURN_END, glm::radians(-angle + 90.0f), 0.5f);

		createWall(glm::vec3(0.5, 0.5, 0.0), glm::vec3(RIGHT_TURN_PERPENDICULAR_R.x, 0.5, RIGHT_TURN_PERPENDICULAR_R.y), glm::vec3(0.5, -0.5, 0.0), glm::vec3(RIGHT_TURN_PERPENDICULAR_R.x, -0.5, RIGHT_TURN_PERPENDICULAR_R.y));

		// Front join
		const glm::vec2& LEFT_TURN_PERPENDICULAR_R = getLineEndPos(LEFT_TURN_END, glm::radians(angle - 90.0f), 0.5f);
		const glm::vec2& RIGHT_TURN_PERPENDICULAR_L = getLineEndPos(RIGHT_TURN_END, glm::radians(-angle - 90.0f), 0.5f);

		createWall(
			glm::vec3(LEFT_TURN_PERPENDICULAR_R.x, 0.5, LEFT_TURN_PERPENDICULAR_R.y), 
			glm::vec3(RIGHT_TURN_PERPENDICULAR_L.x, 0.5, RIGHT_TURN_PERPENDICULAR_L.y),
			glm::vec3(LEFT_TURN_PERPENDICULAR_R.x, -0.5, LEFT_TURN_PERPENDICULAR_R.y),
			glm::vec3(RIGHT_TURN_PERPENDICULAR_L.x, -0.5, RIGHT_TURN_PERPENDICULAR_L.y)
		);

		// Geometry blurring
		splitMeshTriangles(2);
	}	

	~TunnelIntersectionMesh() { }

private:
	const glm::vec2 getLineEndPos(const glm::vec2& startPos, const float radians, const float lineLength)
	{
		return glm::vec2(startPos.x + (lineLength * std::cos(radians)), startPos.y + (lineLength * std::sin(radians)));
	}	
	
	void createFloor(const float Y_POS, const float angle)
	{
		mMesh.addVertex(glm::vec3(-0.5, Y_POS, 0.0), glm::vec2(0.0, 0.0)); // top left
		mMesh.addVertex(glm::vec3(0.5, Y_POS, 0.0), glm::vec2(1.0, 0.0)); // top right
		mMesh.addVertex(glm::vec3(0.5, Y_POS, 1.0), glm::vec2(1.0, 1.0)); // bottom right
		mMesh.addVertex(glm::vec3(-0.5, Y_POS, 0.0), glm::vec2(0.0, 0.0)); // top left
		mMesh.addVertex(glm::vec3(0.5, Y_POS, 1.0), glm::vec2(1.0, 1.0)); // bottom right
		mMesh.addVertex(glm::vec3(-0.5, Y_POS, 1.0), glm::vec2(0.0, 1.0)); // bottom left

		const glm::vec2 START_POS = glm::vec2(0.0, 0.0);

		// Left side
		const glm::vec2& LEFT_TURN_END = -getLineEndPos(START_POS, glm::radians(angle), 1.0f);
		const glm::vec2& LEFT_TURN_PERPENDICULAR_L = getLineEndPos(LEFT_TURN_END, glm::radians(angle + 90.0f), 0.5f);
		const glm::vec2& LEFT_TURN_PERPENDICULAR_R = getLineEndPos(LEFT_TURN_END, glm::radians(angle - 90.0f), 0.5f);

		mMesh.addVertex(glm::vec3(LEFT_TURN_PERPENDICULAR_L.x, Y_POS, LEFT_TURN_PERPENDICULAR_L.y), glm::vec2(1.0, 0.0)); // left join
		mMesh.addVertex(glm::vec3(LEFT_TURN_PERPENDICULAR_R.x, Y_POS, LEFT_TURN_PERPENDICULAR_R.y), glm::vec2(1.0, 0.0)); // right join
		mMesh.addVertex(glm::vec3(-0.5, Y_POS, 0.0), glm::vec2(0.0, 0.0)); // center join		

		// Right side
		glm::vec2 RIGHT_TURN_END = getLineEndPos(START_POS, glm::radians(angle), 1.0f);
		RIGHT_TURN_END.y = -RIGHT_TURN_END.y;

		const glm::vec2& RIGHT_TURN_PERPENDICULAR_L = getLineEndPos(RIGHT_TURN_END, glm::radians(-angle - 90.0f), 0.5f);
		const glm::vec2& RIGHT_TURN_PERPENDICULAR_R = getLineEndPos(RIGHT_TURN_END, glm::radians(-angle + 90.0f), 0.5f);

		mMesh.addVertex(glm::vec3(RIGHT_TURN_PERPENDICULAR_L.x, Y_POS, RIGHT_TURN_PERPENDICULAR_L.y), glm::vec2(1.0, 0.0)); // left join
		mMesh.addVertex(glm::vec3(RIGHT_TURN_PERPENDICULAR_R.x, Y_POS, RIGHT_TURN_PERPENDICULAR_R.y), glm::vec2(1.0, 0.0)); // right join
		mMesh.addVertex(glm::vec3(0.5, Y_POS, 0.0), glm::vec2(0.0, 0.0)); // center join

		// Center floor - left
		mMesh.addVertex(glm::vec3(LEFT_TURN_PERPENDICULAR_R.x, Y_POS, LEFT_TURN_PERPENDICULAR_R.y), glm::vec2(1.0, 0.0)); // right join
		mMesh.addVertex(glm::vec3(0.5, Y_POS, 0.0), glm::vec2(0.0, 0.0)); // center join - right
		mMesh.addVertex(glm::vec3(-0.5, Y_POS, 0.0), glm::vec2(0.0, 0.0)); // center join - left		

		// Center floor - Right
		mMesh.addVertex(glm::vec3(LEFT_TURN_PERPENDICULAR_R.x, Y_POS, LEFT_TURN_PERPENDICULAR_R.y), glm::vec2(1.0, 0.0)); // right join
		mMesh.addVertex(glm::vec3(RIGHT_TURN_PERPENDICULAR_L.x, Y_POS, RIGHT_TURN_PERPENDICULAR_L.y), glm::vec2(1.0, 0.0)); // left join
		mMesh.addVertex(glm::vec3(0.5, Y_POS, 0.0), glm::vec2(0.0, 0.0)); // center join - right
	}

	void createWall(const glm::vec3 topLeft, const glm::vec3 topRight, const glm::vec3 bottomLeft, const glm::vec3 bottomRight)
	{
		mMesh.addVertex(topLeft, glm::vec2(0.0, 0.0)); // top left
		mMesh.addVertex(topRight, glm::vec2(1.0, 0.0)); // top right
		mMesh.addVertex(bottomRight, glm::vec2(1.0, 1.0)); // bottom right

		mMesh.addVertex(topLeft, glm::vec2(0.0, 0.0)); // top left
		mMesh.addVertex(bottomRight, glm::vec2(1.0, 1.0)); // bottom right
		mMesh.addVertex(bottomLeft, glm::vec2(0.0, 1.0)); // bottom left	
	}
};