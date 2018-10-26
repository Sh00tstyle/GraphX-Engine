#include "Scene.h"

#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../Engine/Window.h"
#include "../Engine/World.h"
#include "../Engine/System.h"
#include "../Engine/EntityManager.h"

#include "../Systems/MovementSystem.h"

#include "../Utility/Time.h"
#include "../Utility/Input.h"

Scene::Scene() {
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
	std::cout << "Initializing Engine" << std::endl;

	_window = new Window(1280, 720, "Rendering Engine", 4);
	_world = new World();

	_initializeScene();
}

void Scene::run() {
	while(_window->isOpen()) {
		Time::UpdateTime();

		Input::ProcessInput();

		update();
		render();

		_window->swapBuffers();
		glfwPollEvents();
	}
}

void Scene::update() {
}

void Scene::render() {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
