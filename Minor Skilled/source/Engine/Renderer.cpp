#include "Renderer.h"

#include <map>

#include <glad/glad.h> //NOTE: glad needs to the be included BEFORE glfw, throws errors otherwise
#include <GLFW/glfw3.h>
#include <glm\glm.hpp>

#include "../Engine/Node.h"
#include "../Engine/Transform.h"
#include "../Engine/Material.h"
#include "../Engine/Model.h"

#include "../Components/LightComponent.h"
#include "../Components/CameraComponent.h"
#include "../Components/RenderComponent.h"

#include "../Utility/ComponentType.h"

Renderer::Renderer() {
}

Renderer::~Renderer() {
}

void Renderer::render(std::vector<Node*>& renderables, std::vector<Node*>& lights, std::vector<Node*>& cameras) {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	std::map<RenderComponent*, glm::mat4> renderComponents;
	std::map<CameraComponent*, glm::mat4> cameraComponents;
	std::map<LightComponent*, glm::vec3> lightComponents;

	//fill collections with data (cheating a little by matching the components with their needed matrix/position)
	for(unsigned int i = 0; i < renderables.size(); i++) {
		renderComponents[(RenderComponent*)renderables[i]->getComponent(ComponentType::Render)] = renderables[i]->getTransform()->worldTransform;
	}

	for(unsigned int i = 0; i < cameras.size(); i++) {
		cameraComponents[(CameraComponent*)cameras[i]->getComponent(ComponentType::Camera)] = cameras[i]->getTransform()->worldTransform;
	}

	for(unsigned int i = 0; i < lights.size(); i++) {
		lightComponents[(LightComponent*)lights[i]->getComponent(ComponentType::Light)] = lights[i]->getTransform()->getWorldPosition();
	}

	//render each camera
	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	glm::vec3 cameraPos;

	glm::mat4 modelMatrix;
	Material* material;
	Model* model;

	for(std::map<CameraComponent*, glm::mat4>::iterator cameraIt = cameraComponents.begin(); cameraIt != cameraComponents.end(); cameraIt++) {
		//get relevant properties from the camera
		projectionMatrix = cameraIt->first->projectionMatrix;
		viewMatrix = glm::inverse(cameraIt->second);
		cameraPos = glm::vec3(cameraIt->second[3]);

		//render each renderable
		for(std::map<RenderComponent*, glm::mat4>::iterator renderIt = renderComponents.begin(); renderIt != renderComponents.end(); renderIt++) {
			//get renderable properties
			modelMatrix = renderIt->second;
			material = renderIt->first->material;
			model = renderIt->first->model;

			material->draw(modelMatrix, viewMatrix, projectionMatrix, cameraPos, lightComponents);
			model->draw();
		}
	}
}
