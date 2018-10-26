#include "RenderComponent.h"

RenderComponent::RenderComponent() {
}

RenderComponent::~RenderComponent() {
	delete material;
	delete model;
}
