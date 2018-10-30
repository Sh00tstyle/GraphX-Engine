#include "LightComponent.h"

#include "../Engine/Node.h"

LightComponent::LightComponent(LightType type):Component(ComponentType::Light), lightType(type) {
}

LightComponent::~LightComponent() {
}

void LightComponent::update() {
}
