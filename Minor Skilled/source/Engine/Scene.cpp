#include "Scene.h"

#include <iostream>
#include <string>
#include <bitset>

#include <glad/glad.h> //NOTE: glad needs to the be included BEFORE glfw, throws errors otherwise
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION //NOTE: has to be done once in the project BEFORE including std_image.h
#include "../Engine/stb_image.h"

#include "../Engine/Window.h"
#include "../Engine/World.h"
#include "../Engine/Entity.h"
#include "../Engine/EntityManager.h"

#include "../Components/TransformComponent.h"
#include "../Components/CameraComponent.h"

#include "../Systems/CameraSystem.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"

#include "../Utility/Time.h"
#include "../Utility/Input.h"
#include "../Utility/ComponentType.h"

Scene::Scene():_renderMask(ComponentType::Render | ComponentType::Transform) {
}

Scene::~Scene() {
	delete _window;
	delete _world;

	for(unsigned int i = 0; i < _systems.size(); i++) {
		delete _systems[i];
	}

	EntityManager::DeleteEntities();
}

void Scene::initialize() {
	std::cout << "---Initializing Engine---" << std::endl;

	_window = new Window(1280, 720, "Rendering Engine", 4);
	_world = new World(); //scene graph

	//add basic systems
	CameraSystem* cameraSystem = new CameraSystem();
	_systems.push_back(cameraSystem);

	RenderSystem* renderSystem = new RenderSystem();
	_systems.push_back(renderSystem);

	_initializeScene();
	
	std::cout << "---Engine initialized---" << std::endl;
}

void Scene::run() {
	while(_window->isOpen()) {
		Time::UpdateTime();

		Input::ProcessInput();

		_update();
		_render();

		_window->swapBuffers();
		glfwPollEvents();
	}
}

void Scene::_update() {
	std::vector<Entity*> transformEntities = EntityManager::GetEntitiesByMask(ComponentType::Transform);
	TransformComponent* transformComponent;

	//decompose all transforms before using them in the systems (experimental)
	for(unsigned int i = 0; i < transformEntities.size(); i++) {
		transformComponent = (TransformComponent*)transformEntities[i]->getComponent(ComponentType::Transform);
		transformComponent->decompose();
	}

	//call update for all registered systems
	for(unsigned int i = 0; i < _systems.size(); i++) {
		_systems[i]->update();
	}

	//update scene graph (calculates world transforms only)
	_world->update(); 

	//resets the last mouse pos back to the current mouse pos
	Input::ResetMousePos(); 
}

void Scene::_render() {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Entity* cameraEntity = EntityManager::GetEntityByName("camera");
	TransformComponent* cameraTransformComponent = (TransformComponent*)cameraEntity->getComponent(ComponentType::Transform);
	CameraComponent* cameraComponent = (CameraComponent*)cameraEntity->getComponent(ComponentType::Camera);

	glm::mat4 cameraModel = cameraTransformComponent->worldTransform;
	glm::mat4 projectionMatrix = cameraComponent->projectionMatrix;

	for(unsigned int i = 0; i < _systems.size(); i++) {
		_systems[i]->render(cameraModel, projectionMatrix);
	}
}