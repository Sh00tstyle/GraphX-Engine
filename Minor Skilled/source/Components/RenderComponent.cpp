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

void RenderComponent::update(std::vector<Node*>& renderables, std::vector<Node*>& lights, std::vector<Node*>& cameras) {
	renderables.push_back(_owner);
}
