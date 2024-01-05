#pragma once

#include <glm/glm.hpp>

class GameObject
{
public:
	GameObject()
	{
		position = glm::vec3(0.0, 0.0, 0.0);
		rotation = glm::vec3(0.0, 0.0, 0.0);
		scale = glm::vec3(1.0, 1.0, 1.0);
	}

	void SetPosition(glm::vec3 newPosition)
	{
		position = newPosition;
	}	
	
	glm::vec3 GetPosition()
	{
		return position;
	}	
	
	void SetRotation(glm::vec3 newRotation)
	{
		rotation = newRotation;
	}	
	
	glm::vec3 GetRotation()
	{
		return rotation;
	}	
	
	void SetScale(glm::vec3 newScale)
	{
		scale = newScale;
	}	
	
	glm::vec3 GetScale()
	{
		return scale;
	}

private:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};