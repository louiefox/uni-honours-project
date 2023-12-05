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
		mesh = Mesh();

		mesh.AddVertex(glm::vec3(-0.5, -0.5, 0.0), glm::vec2(0.0, 0.0)); // top left
		mesh.AddVertex(glm::vec3(0.5, -0.5, 0.0), glm::vec2(1.0, 0.0)); // top right
		mesh.AddVertex(glm::vec3(0.5, 0.5, 0.0), glm::vec2(1.0, 1.0)); // bottom right
		mesh.AddVertex(glm::vec3(-0.5, -0.5, 0.0), glm::vec2(0.0, 0.0)); // top left
		mesh.AddVertex(glm::vec3(0.5, 0.5, 0.0), glm::vec2(1.0, 1.0)); // bottom right
		mesh.AddVertex(glm::vec3(-0.5, 0.5, 0.0), glm::vec2(0.0, 1.0)); // bottom left

		mesh.Generate();
	}	

	~TunnelIntersectionMesh()
	{

	}

	void Draw()
	{
		mesh.Draw();
	}

private:
	Mesh mesh;
};