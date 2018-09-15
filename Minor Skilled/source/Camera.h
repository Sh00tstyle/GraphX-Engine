#ifndef CAMERA_H
#define CAMERA_H

#include <glad\glad.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <vector>

enum CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
	public:
		//camera attributes
		glm::vec3 position;
		glm::vec3 front;
		glm::vec3 up;
		glm::vec3 right;
		glm::vec3 worldUp;

		//euler angles
		float yaw;
		float pitch;

		//camera options
		float movementSpeed;
		float mouseSensitivity;
		float zoom;

		Camera(glm::vec3 pPosition = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 pUp = glm::vec3(0.0f, 1.0f, 0.0f), float pYaw = YAW, float pPitch = PITCH);
		Camera(float pPosX, float pPosY, float pPosZ, float pUpX, float pUpY, float pUpZ, float pYaw, float pPitch);

		glm::mat4 getViewMatrix();
		void processKeyboard(CameraMovement direction, float deltaTime);
		void processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true);
		void processMouseScroll(float yOffset);

	private:
		void _updateCameraVectors();
};

#endif