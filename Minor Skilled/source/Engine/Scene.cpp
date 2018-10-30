#include "Scene.h"

#include <iostream>
#include <string>
#include <bitset>

#define STB_IMAGE_IMPLEMENTATION //NOTE: has to be done once in the project BEFORE including std_image.h
#include "../Engine/stb_image.h"

#include "../Engine/Node.h"
#include "../Engine/Window.h"
#include "../Engine/World.h"
#include "../Engine/Renderer.h"

#include "../Utility/Time.h"
#include "../Utility/Input.h"
#include "../Utility/ComponentType.h"

Scene::Scene() {
}

Scene::~Scene() {
	delete _window;
	delete _world;
}

void Scene::initialize() {
	std::cout << "---Initializing Engine---" << std::endl;

	_window = new Window(1280, 720, "GraphX Engine", 4);
	_world = new World(); //scene graph
	_renderer = new Renderer();

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
		_window->pollEvents();
	}
}

void Scene::_update() {
	//clear vectors
	_renderables.clear();
	_lights.clear();
	_cameras.clear();

	//update scene graph and pass in vectors to fill
	_world->update(_renderables, _lights, _cameras); 

	//resets the last mouse pos back to the current mouse pos
	Input::ResetMousePos(); 
}

void Scene::_render() {
	_renderer->render(_renderables, _lights, _cameras);
}