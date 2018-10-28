#include "TransformComponent.h"

TransformComponent::TransformComponent():Component(ComponentType::Transform), worldTransform(glm::mat4(1.0f)) {
}

TransformComponent::~TransformComponent() {
}

void TransformComponent::decompose() {
	//matrix decomposition (dark magic): https://stackoverflow.com/questions/17918033/glm-decompose-mat4-into-translation-and-rotation
	glm::decompose(localTransform, _localScale, _localRotation, _localPosition, _localSkew, _localPerspective);
	glm::decompose(worldTransform, _worldScale, _worldRotation, _worldPosition, _worldSkew, _worldPerspective);

	_localRotation = glm::conjugate(_localRotation);
	_worldRotation = glm::conjugate(_worldRotation);
}

glm::vec3 TransformComponent::getLocalScale() {
	return _localScale;
}

glm::quat TransformComponent::getLocalRotation() {
	return _localRotation;
}

glm::vec3 TransformComponent::getLocalPosition(bool decomposed) {
	if(decomposed) return _localPosition;
	else return glm::vec3(localTransform[3]);
}

glm::vec3 TransformComponent::getLocalSkew() {
	return _localSkew;
}

glm::vec3 TransformComponent::getLocalPerspective() {
	return _localPerspective;
}

glm::vec3 TransformComponent::getWorldScale() {
	return _worldScale;
}

glm::quat TransformComponent::getWorldRotation() {
	return _worldRotation;
}

glm::vec3 TransformComponent::getWorldPosition(bool decomposed) {
	if(decomposed) return _worldPosition;
	else return glm::vec3(worldTransform[3]);
}

glm::vec3 TransformComponent::getWorldSkew() {
	return _worldSkew;
}

glm::vec3 TransformComponent::getWorldPerspective() {
	return _worldPerspective;
}