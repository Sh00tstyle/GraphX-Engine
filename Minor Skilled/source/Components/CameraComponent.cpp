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
_projectionMatrix(projectionMatrix), fieldOfView(fieldOfView), _nearPlane(nearPlane), _farPlane(farPlane), movementSpeed(movementSpeed), rotationSpeed(rotationSpeed), _transform(nullptr), 
_startTransformMatrix(glm::mat4(1.0f)), _startEulerRotation(glm::vec3(0.0f)), _firstTransform(true) {
}

CameraComponent::~CameraComponent() {
}

float CameraComponent::getNearPlane() {
	return _nearPlane;
}

float CameraComponent::getFarPlane() {
	return _farPlane;
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

	//check if the camera has been reset
	_checkForCameraReset();

	if(Input::GetMouse(MouseButton::Right)) {
		double deltaTime = Time::DeltaTime;

		//mouse offset
		glm::dvec2 mouseOffset = Input::GetLastMousePos() - Input::GetCurrentMousePos();

		//movement
		glm::vec3 translation = glm::vec3(0.0f);

		if(Input::GetKey(Key::W)) translation.z = -movementSpeed * deltaTime;
		else if(Input::GetKey(Key::S)) translation.z = movementSpeed * deltaTime;

		if(Input::GetKey(Key::A)) translation.x = -movementSpeed * deltaTime;
		else if(Input::GetKey(Key::D)) translation.x = movementSpeed * deltaTime;

		if(Input::GetKey(Key::SPACE)) translation.y = movementSpeed * deltaTime;
		else if(Input::GetKey(Key::LSHIFT)) translation.y = -movementSpeed * deltaTime;

		//rotation
		glm::vec3 eulerAngles = _transform->getLocalEuler();

		if(std::abs(mouseOffset.x) >= 0.01f) eulerAngles.y += mouseOffset.x * rotationSpeed * deltaTime;
		if(std::abs(mouseOffset.y) >= 0.01f) eulerAngles.x += mouseOffset.y * rotationSpeed * deltaTime;

		//reconstruct transform and apply to component
		glm::vec3 localPos = _transform->getLocalPosition();

		_transform->localTransform = glm::mat4(1.0f);
		_transform->translate(localPos); //translate to old position
		_transform->setEulerRotation(eulerAngles.x, eulerAngles.y, 0.0f); //rotate to new position
		_transform->translate(translation); //translate by movement input
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

void CameraComponent::_checkForCameraReset() {
	if(_firstTransform) {
		//lazy initialize as soon as it gets updated the first time
		_startTransformMatrix = _transform->localTransform;
		_startEulerRotation = _transform->getLocalEuler();

		_firstTransform = false;
	}

	if(Input::GetKeyDown(Key::RSHIFT)) {
		//reset to initial state
		_transform->localTransform = _startTransformMatrix;
		_transform->setEulerRotation(_startEulerRotation.x, _startEulerRotation.y, _startEulerRotation.z);
	}
}
