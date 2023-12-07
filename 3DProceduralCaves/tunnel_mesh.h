#pragma once

#include <array>

#include "mesh.h"
#include "game_object.h"

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
		mMesh.addVertex(topLeft, glm::vec2(0.0, 0.0)); // top left
		mMesh.addVertex(topRight, glm::vec2(1.0, 0.0)); // top right
		mMesh.addVertex(bottomRight, glm::vec2(1.0, 1.0)); // bottom right

		mMesh.addVertex(topLeft, glm::vec2(0.0, 0.0)); // top left
		mMesh.addVertex(bottomRight, glm::vec2(1.0, 1.0)); // bottom right
		mMesh.addVertex(bottomLeft, glm::vec2(0.0, 1.0)); // bottom left	
	}
};