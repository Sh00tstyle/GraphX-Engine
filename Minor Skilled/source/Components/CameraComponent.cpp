#include "CameraComponent.h"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Engine/Node.h"
#include "../Engine/Transform.h"
#include "../Engine/Window.h"
#include "../Engine//Transform.h"
#include "../Engine/Debug.h"

#include "../Components/LightComponent.h"

#include "../Utility/Input.h"
#include "../Utility/Time.h"

CameraComponent::CameraComponent(glm::mat4 projectionMatrix, float fieldOfView, float nearPlane, float farPlane, float movementSpeed, float rotationSpeed) : Component(ComponentType::Camera),
_projectionMatrix(projectionMatrix), fieldOfView(fieldOfView), _nearPlane(nearPlane), _farPlane(farPlane), movementSpeed(movementSpeed), rotationSpeed(rotationSpeed), _transform(nullptr), _rotX(glm::mat4(1.0f)), _rotY(glm::mat4(1.0f)), _startTransformMatrix(glm::mat4(1.0f)), _firstTransform(true) {
}

CameraComponent::~CameraComponent() {
}

glm::mat4 CameraComponent::getProjectionMatrix() {
	return _projectionMatrix;
}

glm::mat4 CameraComponent::getViewMatrix() {
	_transform = _owner->getTransform();
	glm::mat4 modelMatrix = _transform->worldTransform;
	glm::mat4 viewMatrix = glm::inverse(modelMatrix);

	return viewMatrix;
}

glm::mat4 CameraComponent::getPreviousViewProjectionMatrix() {
	glm::mat4 previousViewProjection = _previousViewProjectionMatrix;

	//update class member to new one
	_previousViewProjectionMatrix = getProjectionMatrix() *  getViewMatrix(); //store for next frame

	return previousViewProjection;
}

void CameraComponent::setOwner(Node * owner) {
	_owner = owner; //initialize owner

	_transform = _owner->getTransform();

	//also initialize the previous view projection matrix
	_previousViewProjectionMatrix = getProjectionMatrix() * getViewMatrix();
}

void CameraComponent::update() {
	//update projection matrix if needed
	_updateProjectionMatrix();

	//process movement if the right mouse button is pressed
	_transform = _owner->getTransform();

	//check if the camera has been reset
	_checkForCameraReset(_transform);

	if(Input::GetMouse(MouseButton::Right)) {
		//mouse offset
		glm::vec2 mouseOffset = Input::GetLastMousePos() - Input::GetCurrentMousePos();

		//movement
		glm::vec3 translation = glm::vec3(0.0f);

		if(Input::GetKey(Key::W)) translation.z = -movementSpeed * Time::DeltaTime;
		if(Input::GetKey(Key::S)) translation.z = movementSpeed * Time::DeltaTime;
		if(Input::GetKey(Key::A)) translation.x = -movementSpeed * Time::DeltaTime;
		if(Input::GetKey(Key::D)) translation.x = movementSpeed * Time::DeltaTime;
		if(Input::GetKey(Key::SPACE)) translation.y = movementSpeed * Time::DeltaTime;
		if(Input::GetKey(Key::LSHIFT)) translation.y = -movementSpeed * Time::DeltaTime;

		//rotation
		glm::mat4 yRotation = _rotY;
		glm::mat4 xRotation = _rotX;

		if(std::abs(mouseOffset.x) >= 0.01f) {
			yRotation = glm::rotate(yRotation, glm::radians(mouseOffset.x * rotationSpeed * Time::DeltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
			_rotY = yRotation;
		}

		if(std::abs(mouseOffset.y) >= 0.01f) {
			xRotation = glm::rotate(xRotation, glm::radians(mouseOffset.y * rotationSpeed * Time::DeltaTime), glm::vec3(1.0f, 0.0f, 0.0f));
			_rotX = xRotation;
		}

		//reconstruct transform and apply to component
		glm::vec3 localPos = _transform->getLocalPosition();

		glm::mat4 newTransform = glm::mat4(1.0f);

		newTransform = glm::translate(newTransform, localPos);
		newTransform = newTransform * yRotation * xRotation; //first rotate over the x axis and the over the y axis
		newTransform = glm::translate(newTransform, translation);

		_transform->localTransform = newTransform;
	}

	//update lights attached to the camera
	if(_owner->hasComponent(ComponentType::Light)) {
		LightComponent* lightComponent = (LightComponent*)_owner->getComponent(ComponentType::Light);
		if(lightComponent->lightType != LightType::Spot) return; //no need to update the light component when the attached light is no spotlight

		glm::vec3 cameraForward = _transform->localTransform[2]; //second row represents the (local) forward vector
		lightComponent->lightDirection = cameraForward;
	}
}

void CameraComponent::_updateProjectionMatrix() {
	_projectionMatrix = glm::perspective(glm::radians(fieldOfView), (float)Window::ScreenWidth / (float)Window::ScreenHeight, _nearPlane, _farPlane);
}

void CameraComponent::_checkForCameraReset(Transform* transform) {
	if(_firstTransform) {
		//lazy initialize as soon as it gets updated the first time
		_startTransformMatrix = transform->localTransform;

		_firstTransform = false;
	}

	if(Input::GetKeyDown(Key::RSHIFT)) {
		transform->localTransform = _startTransformMatrix; //reset to initial state

		//reset rotations as well
		_rotX = glm::mat4(1.0f);
		_rotY = glm::mat4(1.0f);
	}
}
