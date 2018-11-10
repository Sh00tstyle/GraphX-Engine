#include "LightComponent.h"

#include "../Engine/Node.h"

const unsigned int LightComponent::LightAmount = 10;

LightComponent::LightComponent(LightType type):Component(ComponentType::Light), lightType(type) {
}

LightComponent::~LightComponent() {
}

void LightComponent::update() {
}
