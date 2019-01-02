#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H

#include <glm/glm.hpp>

#include "../Engine/Component.h"

class Node;
class Transform;

class CameraComponent : public Component {
	public:
		CameraComponent(glm::mat4 projectionMatrix, float fieldOfView, float nearPlane, float farPlane, float movementSpeed, float rotationSpeed);
		~CameraComponent();

		float fieldOfView;
		float movementSpeed; 
		float rotationSpeed;

		float getNearPlane();
		float getFarPlane();
		glm::mat4 getProjectionMatrix();
		glm::mat4 getViewMatrix();
		glm::mat4 getPreviousViewProjectionMatrix();

		virtual void setOwner(Node* owner);

		virtual void update();

	private:
		Transform* _transform;

		glm::mat4 _projectionMatrix;
		glm::mat4 _previousViewProjectionMatrix;

		glm::mat4 _startTransformMatrix;
		glm::vec3 _startEulerRotation;
		bool _firstTransform;

		float _nearPlane;
		float _farPlane;

		void _updateProjectionMatrix();
		void _checkForCameraReset();
};

#endif