#pragma once

#include <glm/glm.hpp>

class GameObject
{
public:
	GameObject();

	void SetPosition(glm::vec3 newPosition);
	glm::vec3 GetPosition();
	
	void SetRotation(glm::vec3 newRotation);
	glm::vec3 GetRotation();
	
	void SetScale(glm::vec3 newScale);
	glm::vec3 GetScale();

private:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};