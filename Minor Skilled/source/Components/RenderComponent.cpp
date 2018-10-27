#include "RenderComponent.h"

RenderComponent::RenderComponent():Component(ComponentType::Render) {
}

RenderComponent::~RenderComponent() {
	delete material;
	delete model;
}
