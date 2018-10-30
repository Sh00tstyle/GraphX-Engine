#include "RenderComponent.h"

#include "../Engine/Material.h"
#include "../Engine/Model.h"
#include "../Engine/Node.h"

RenderComponent::RenderComponent(Model * model, Material * material):Component(ComponentType::Render), model(model), material(material) {
}

RenderComponent::~RenderComponent() {
	delete material;
	delete model;
}

void RenderComponent::update() {
}
