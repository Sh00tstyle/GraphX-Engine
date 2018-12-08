#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H

#include <glm/glm.hpp>

#include "../Engine/Component.h"

class Node;

class CameraComponent : public Component {
	public:
		CameraComponent(glm::mat4 projectionMatrix, float fieldOfView, float nearPlane, float farPlane, float movementSpeed, float rotationSpeed);
		~CameraComponent();

		glm::mat4 projectionMatrix;
		float fieldOfView;
		float movementSpeed; 
		float rotationSpeed;
		glm::mat4 rotX;
		glm::mat4 rotY;

		virtual void update();

	private:
		void _updateProjectionMatrix();

		float _nearPlane;
		float _farPlane;
};

#endif