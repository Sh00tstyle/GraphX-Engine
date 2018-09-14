#include "Camera.h"

Camera::Camera(glm::vec3 pPosition, glm::vec3 pUp, float pYaw, float pPitch) : 
	front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM) {

	position = pPosition;
	worldUp = pUp;
	yaw = pYaw;
	pitch = pPitch;
	_updateCameraVectors();
}

Camera::Camera(float pPosX, float pPosY, float pPosZ, float pUpX, float pUpY, float pUpZ, float pYaw, float pPitch) 
	: front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM) {

	position = glm::vec3(pPosX, pPosY, pPosZ);
	worldUp = glm::vec3(pUpX, pUpY, pUpZ);
	yaw = pYaw;
	pitch = pPitch;
	_updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(CameraMovement direction, float deltaTime) {
	float velocity = movementSpeed * deltaTime;
	if(direction == FORWARD)
		position += front * velocity;
	if(direction == BACKWARD)
		position -= front * velocity;
	if(direction == LEFT)
		position -= right * velocity;
	if(direction == RIGHT)
		position += right * velocity;
}

void Camera::processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch) {
	xOffset *= mouseSensitivity;
	yOffset *= mouseSensitivity;

	yaw += xOffset;
	pitch += yOffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if(constrainPitch) {
		if(pitch > 89.0f) pitch = 89.0f;
		if(pitch < -89.0f) pitch = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Euler angles
	_updateCameraVectors();
}

void Camera::processMouseScroll(float yOffset) {
	//apply offset to zoom
	if(zoom >= 1.0f && zoom <= 45.0f) zoom -= (float)yOffset;

	//set limits
	if(zoom <= 1.0f) zoom = 1.0f;
	if(zoom >= 45.0f) zoom = 45.0f;
}

void Camera::_updateCameraVectors() {
	// Calculate the new Front vector
	glm::vec3 newFront;
	newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	newFront.y = sin(glm::radians(pitch));
	newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(newFront);

	// Also re-calculate the Right and Up vector
	right = glm::normalize(glm::cross(front, worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	up = glm::normalize(glm::cross(right, front));
}