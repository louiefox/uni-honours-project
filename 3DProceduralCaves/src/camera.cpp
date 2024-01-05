#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 startPosition)
{
	cameraPos = startPosition;
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void Camera::setFOV(float newValue)
{
	fov = std::max(std::min(newValue, 45.0f), 1.0f);
}

float Camera::getFOV()
{
	return fov;
}

void Camera::applyMouseInput(float xOffset, float yOffset)
{
	// apply sensitivity
	const float sensitivity = 0.05f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	yaw += xOffset;
	pitch = std::max(std::min(pitch + yOffset, 89.0f), -89.0f); // lock pitch angles

	// adjust camera front normal vector based on angles
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

void Camera::applyKeyboardInput(CameraDirections direction, float speed)
{
	switch (direction)
	{
	case FORWARD:
		cameraPos += speed * cameraFront;
		break;
	case RIGHT:
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
		break;
	}

	//cameraPos.y = 0.0f; // disable vertical movement
}