#include "RenderSystem.h"

#include <map>

#include <glm/glm.hpp>

#include "../Engine/Entity.h"
#include "../Engine/Material.h"
#include "../Engine/Model.h"
#include "../Engine/EntityManager.h"

#include "../Components/TransformComponent.h"
#include "../Components/RenderComponent.h"
#include "../Components/LightComponent.h"

#include "../Utility/ComponentType.h"

RenderSystem::RenderSystem():System(ComponentType::Transform | ComponentType::Render) {
}

RenderSystem::~RenderSystem() {
}

void RenderSystem::render(glm::mat4 cameraModel, glm::mat4 projectionMatrix) {
	//get all entities that have a light and transform component and add the components to a vector
	std::vector<Entity*> lightEntities = EntityManager::GetEntitiesByMask(ComponentType::Light | ComponentType::Transform);
	std::map<LightComponent*, glm::vec3> lights; //using a little trick here to store the light properties and their respective positions together

	TransformComponent* transformComponent;

	for(unsigned int i = 0; i < lightEntities.size(); i++) {
		transformComponent = (TransformComponent*)lightEntities[i]->getComponent(ComponentType::Transform);

		lights[(LightComponent*)lightEntities[i]->getComponent(ComponentType::Light)] = transformComponent->getLocalPosition();
	}

	//get all entities that have a transform and a render component
	std::vector<Entity*> renderEntities = EntityManager::GetEntitiesByMask(_requirements);

	RenderComponent* renderComponent;

	glm::mat4 viewMatrix = glm::inverse(cameraModel);
	glm::vec3 cameraPos = glm::vec3(cameraModel[3]);

	for(unsigned int i = 0; i < renderEntities.size(); i++) {
		transformComponent = (TransformComponent*)renderEntities[i]->getComponent(ComponentType::Transform);
		renderComponent = (RenderComponent*)renderEntities[i]->getComponent(ComponentType::Render);

		renderComponent->material->draw(transformComponent->worldTransform, viewMatrix, projectionMatrix, cameraPos, lights);
		renderComponent->model->draw();
	}
}