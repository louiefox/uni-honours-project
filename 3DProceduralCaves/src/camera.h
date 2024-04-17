#pragma once

#include <glm/glm.hpp>

enum CameraDirections {
	FORWARD,
	RIGHT
};

class Camera
{
public:
	Camera(glm::vec3 startPosition);
	
	void setFOV(float newValue);
	float getFOV();
	float getYaw() { return yaw; }
	float getPitch() { return pitch; }

	glm::vec3 getPosition() { return cameraPos; }

	glm::mat4 getViewMatrix();
	
	void applyMouseInput(float xOffset, float yOffset);
	void applyKeyboardInput(CameraDirections direction, float speed);

private:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	float yaw = -90.0f;
	float pitch = 0.0f;
	float fov = 45.0f;
};