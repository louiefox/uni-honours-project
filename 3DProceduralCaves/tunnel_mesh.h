#pragma once

#include <array>

#include "plane.h"
#include "game_object.h"

class TunnelMesh : public GameObject
{
public:
	int SideSquareCount = 16;
	std::array<Plane, 4> Sides = {Plane(SideSquareCount), Plane(SideSquareCount), Plane(SideSquareCount), Plane(SideSquareCount)};

	TunnelMesh() { }

	void draw(Shader& shaderProgram, glm::mat4& modelMat)
	{
		for (int i = 0; i < 4; i++)
		{
			glm::mat4 sideModelMat = modelMat;
			sideModelMat = glm::translate(sideModelMat, glm::vec3(i == 1 ? 0.5 : i == 3 ? -0.5 : 0.0, i == 0 ? 0.5 : i == 2 ? -0.5 : 0.0, 0.0));
			sideModelMat = glm::rotate(sideModelMat, glm::radians(90.0f), glm::vec3(i % 2 == 0 ? 1.0 : 0.0, i % 2 != 0 ? 1.0 : 0.0, 0.0));

			shaderProgram.setMat4("model", sideModelMat);
			Sides[i].draw();
		}
	}

private:
};