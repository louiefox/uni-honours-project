#pragma once

#include <array>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.h"
#include "game_object.h"
#include "shader.h"

class TunnelIntersectionMesh : public GameObject
{
public:
	TunnelIntersectionMesh(float angle) 
	{
		mMesh = Mesh();

		mMesh.addVertex(glm::vec3(-0.5, -0.5, 0.0), glm::vec2(0.0, 0.0)); // top left
		mMesh.addVertex(glm::vec3(0.5, -0.5, 0.0), glm::vec2(1.0, 0.0)); // top right
		mMesh.addVertex(glm::vec3(0.5, 0.5, 0.0), glm::vec2(1.0, 1.0)); // bottom right
		mMesh.addVertex(glm::vec3(-0.5, -0.5, 0.0), glm::vec2(0.0, 0.0)); // top left
		mMesh.addVertex(glm::vec3(0.5, 0.5, 0.0), glm::vec2(1.0, 1.0)); // bottom right
		mMesh.addVertex(glm::vec3(-0.5, 0.5, 0.0), glm::vec2(0.0, 1.0)); // bottom left

		mMesh.generate();
	}	

	~TunnelIntersectionMesh()
	{

	}

	void draw()
	{
		mMesh.draw();
	}

private:
	Mesh mMesh;
};