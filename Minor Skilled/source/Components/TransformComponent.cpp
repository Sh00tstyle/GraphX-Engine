#include "TransformComponent.h"

TransformComponent::TransformComponent():Component(ComponentType::Transform) {
}

TransformComponent::~TransformComponent() {
}

glm::vec3 TransformComponent::getLocalPosition() {
	return glm::vec3(localTransform[3]);
}

glm::vec3 TransformComponent::getWorldPosition() {
	return glm::vec3(worldTransform[3]);
}
