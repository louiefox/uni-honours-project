#include "game_object.h"

#include <glm/glm.hpp>

GameObject::GameObject()
{
	position = glm::vec3(0.0, 0.0, 0.0);
	rotation = glm::vec3(0.0, 0.0, 0.0);
	scale = glm::vec3(1.0, 1.0, 1.0);
}

void GameObject::SetPosition(glm::vec3 newPosition)
{
	position = newPosition;
}

glm::vec3 GameObject::GetPosition()
{
	return position;
}

void GameObject::SetRotation(glm::vec3 newRotation)
{
	rotation = newRotation;
}

glm::vec3 GameObject::GetRotation()
{
	return rotation;
}

void GameObject::SetScale(glm::vec3 newScale)
{
	scale = newScale;
}

glm::vec3 GameObject::GetScale()
{
	return scale;
}