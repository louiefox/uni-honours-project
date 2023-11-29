#pragma once

#include <array>
#include <glm/gtc/matrix_transform.hpp>

#include "plane.h"
#include "game_object.h"
#include "shader.h"

class TunnelIntersectionMesh : public GameObject
{
public:
	TunnelIntersectionMesh(float angle) 
	{
		CreatePlane(glm::vec3(0.0, 0.0, 0.0), glm::vec3(90.0, 0.0, 0.0));

		float lineLength = 1.0f;
		glm::vec2 lineStart = glm::vec2(0.0, -0.5);

		glm::vec2 lineEnd = glm::vec2(lineStart.x - (lineLength * std::cos(glm::radians(angle))), lineStart.y - (lineLength * std::sin(glm::radians(angle))));
		CreatePlane(glm::vec3(lineStart.x + (lineEnd.x - lineStart.x) / 2, 0.0f, lineStart.y + (lineEnd.y - lineStart.y) / 2), glm::vec3(90.0, 0.0, angle));		

		CreatePlane(glm::vec3(lineStart.x - (lineEnd.x - lineStart.x) / 2, 0.0f, lineStart.y + (lineEnd.y - lineStart.y) / 2), glm::vec3(90.0, 0.0, -angle));
	}	

	~TunnelIntersectionMesh()
	{
		for (Plane* plane : planes)
			delete plane;
	}
	
	void CreatePlane(const glm::vec3& position, const glm::vec3& angles)
	{
		planes.push_back(new Plane(16));
		planeOffsets.push_back({position , angles});
	}	

	void Draw(Shader& shaderProgram, glm::mat4& modelMat)
	{
		for (int i = 0; i < planes.size(); i++)
		{
			Plane* plane = planes[i];
			PlaneOffset& offset = planeOffsets[i];

			glm::mat4 sideModelMat = modelMat;
			sideModelMat = glm::translate(sideModelMat, offset.position);
			RotateMatrixByVec3(sideModelMat, offset.angles);

			shaderProgram.setMat4("model", sideModelMat);
			plane->Draw();
		}
	}

private:
	struct PlaneOffset
	{
		glm::vec3 position;
		glm::vec3 angles;
	};

	std::vector<PlaneOffset> planeOffsets;
	std::vector<Plane*> planes;

	void RotateMatrixByVec3(glm::mat4& model, const glm::vec3& rotation)
	{
		model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0, 0.0, 0.0));
		model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0, 1.0, 0.0));
		model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0, 0.0, 1.0));
	}
};