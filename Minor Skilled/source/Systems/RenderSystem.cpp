#include "RenderSystem.h"

#include <glm/glm.hpp>

#include "../Engine/Entity.h"
#include "../Engine/Material.h"
#include "../Engine/Model.h"
#include "../Engine/EntityManager.h"

#include "../Components/TransformComponent.h"
#include "../Components/RenderComponent.h"

#include "../Utility/ComponentType.h"

RenderSystem::RenderSystem():System(ComponentType::Transform | ComponentType::Render) {
}

RenderSystem::~RenderSystem() {
}

void RenderSystem::render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
	std::vector<Entity*> renderEntities = EntityManager::GetEntitiesByMask(_requirements); //get all entities that have a transform and a render component

	TransformComponent* transformComponent;
	RenderComponent* renderComponent;

	for(unsigned int i = 0; i < renderEntities.size(); i++) {
		transformComponent = (TransformComponent*)renderEntities[i]->getComponent(ComponentType::Transform);
		renderComponent = (RenderComponent*)renderEntities[i]->getComponent(ComponentType::Render);

		renderComponent->material->draw(transformComponent->worldTransform, viewMatrix, projectionMatrix);
		renderComponent->model->draw();
	}
}