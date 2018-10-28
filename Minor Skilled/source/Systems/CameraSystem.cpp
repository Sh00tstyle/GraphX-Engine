#include "CameraSystem.h"

#include <iostream>
#include <string>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../Engine/EntityManager.h"
#include "../Engine/Entity.h"

#include "../Components/TransformComponent.h"
#include "../Components/CameraComponent.h"

#include "../Utility/Input.h"
#include "../Utility/Key.h"
#include "../Utility/Time.h"
#include "../Utility/ComponentType.h"

CameraSystem::CameraSystem():System(ComponentType::Transform | ComponentType::Camera) {
}

CameraSystem::~CameraSystem() {
}

void CameraSystem::update() {
	std::vector<Entity*> cameraEntities = EntityManager::GetEntitiesByMask(_requirements); //get all entities that have a transform and a render component

	TransformComponent* transformComponent;
	CameraComponent* cameraComponent;

	glm::vec2 mouseOffset = Input::GetLastMousePos() - Input::GetCurrentMousePos();

	for(unsigned int i = 0; i < cameraEntities.size(); i++) {
		transformComponent = (TransformComponent*)cameraEntities[i]->getComponent(ComponentType::Transform);
		cameraComponent = (CameraComponent*)cameraEntities[i]->getComponent(ComponentType::Camera);

		_processInput(transformComponent, cameraComponent, mouseOffset);
	}
}

void CameraSystem::_processInput(TransformComponent* transformComponent, CameraComponent* cameraComponent, glm::vec2 mouseOffset) {
	//movement
	glm::vec3 translation = glm::vec3(0.0f);
	float movementSpeed = cameraComponent->movementSpeed;

	if(Input::GetKey(Key::W)) translation.z = -movementSpeed * Time::DeltaTime;
	if(Input::GetKey(Key::S)) translation.z = movementSpeed * Time::DeltaTime;
	if(Input::GetKey(Key::A)) translation.x = -movementSpeed * Time::DeltaTime;
	if(Input::GetKey(Key::D)) translation.x = movementSpeed * Time::DeltaTime;
	if(Input::GetKey(Key::SPACE)) translation.y = movementSpeed * Time::DeltaTime;
	if(Input::GetKey(Key::LSHIFT)) translation.y = -movementSpeed * Time::DeltaTime;

	//rotation
	glm::mat4 yRotation = cameraComponent->rotY;
	glm::mat4 xRotation = cameraComponent->rotX;
	float rotationSpeed = cameraComponent->rotationSpeed;

	if(std::abs(mouseOffset.x) >= 1.0f) {
		yRotation = glm::rotate(yRotation, glm::radians(mouseOffset.x * rotationSpeed * Time::DeltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
		cameraComponent->rotY = yRotation;
	}

	if(std::abs(mouseOffset.y) >= 1.0f) {
		xRotation = glm::rotate(xRotation, glm::radians(mouseOffset.y * rotationSpeed * Time::DeltaTime), glm::vec3(1.0f, 0.0f, 0.0f));
		cameraComponent->rotX = xRotation;
	}

	//reconstruct transform and apply to component
	glm::mat4 transform = glm::mat4(1.0f);

	transform = glm::translate(transform, transformComponent->getLocalPosition());
	transform = transform * yRotation * xRotation;
	transform = glm::translate(transform, translation);

	transformComponent->localTransform = transform;
}