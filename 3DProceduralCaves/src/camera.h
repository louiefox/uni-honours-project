#pragma once

enum CameraDirections {
	FORWARD,
	RIGHT
};

class Camera
{
public:
	Camera(glm::vec3 startPosition)
	{
		cameraPos = startPosition;
	}

	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}	
	
	void setFOV(float newValue)
	{
		fov = std::max(std::min(newValue, 45.0f), 1.0f);
	}	
	
	float getFOV()
	{
		return fov;
	}	
	
	void applyMouseInput(float xOffset, float yOffset)
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
	
	void applyKeyboardInput(CameraDirections direction, float speed)
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

private:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	float yaw = -90.0f;
	float pitch = 0.0f;
	float fov = 45.0f;
};