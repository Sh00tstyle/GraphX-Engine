#include "LightComponent.h"

#include "../Engine/Node.h"

LightComponent::LightComponent(LightType type):Component(ComponentType::Light), lightType(type) {
}

LightComponent::~LightComponent() {
}

void LightComponent::update(std::vector<Node*>& renderables, std::vector<Node*>& lights, std::vector<Node*>& cameras) {
	lights.push_back(_owner);
}
