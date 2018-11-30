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
#include "../Engine/Texture.h"

#include "../Components/LightComponent.h"

#include "../Utility/Time.h"
#include "../Utility/Input.h"
#include "../Utility/ComponentType.h"
#include "../Utility/LightType.h"

Scene::Scene() {
}

Scene::~Scene() {
	delete _window;
	delete _world;
	delete _skybox;
	delete _renderer;
}

void Scene::initialize() {
	std::cout << "---Initializing Engine---" << std::endl;

	_window = new Window(1280, 720, "GraphX Engine");
	_world = new World(); //scene graph
	_renderer = new Renderer();

	_skybox = nullptr;
	_mainCamera = nullptr;
	_directionalLight = nullptr;

	_initializeScene();
	
	std::cout << "---Engine initialized---" << std::endl;
}

void Scene::run() {
	while(_window->isOpen()) { //render loop
		Time::UpdateTime();

		Input::ProcessInput();

		_update();
		_render();

		_window->swapBuffers();
		_window->pollEvents();
	}
}

void Scene::_setSkybox(Texture* skybox) {
	if(skybox == nullptr) {
		std::cout << "ERROR: Unable to assign skybox. It was null." << std::endl;
		return;
	}

	_skybox = skybox;
}

void Scene::_setMainCamera(Node* mainCamera) {
	if(!mainCamera->hasComponent(ComponentType::Camera)) {
		std::cout << "ERROR: Unable to assign main camera. It has no camera component." << std::endl;
		return;
	}

	_mainCamera = mainCamera;
}

void Scene::_setDirectionalLight(Node * directionalLight) {
	if(!directionalLight->hasComponent(ComponentType::Light)) {
		std::cout << "ERROR: Unable to assign directional light. It has no light component." << std::endl;
		return;
	} else if(((LightComponent*)directionalLight->getComponent(ComponentType::Light))->lightType != LightType::Directional) {
		std::cout << "ERROR: Unable to assign directional light. Its light type is not DIRECTIONAL." << std::endl;
		return;
	}

	_directionalLight = directionalLight;
}

void Scene::_update() {
	//clear vectors
	_renderables.clear();
	_lights.clear();
	_cameras.clear();

	//update scene graph and pass in vectors to fill
	_world->update(_renderables, _lights, _cameras); 

	//resets the last mouse pos back to the current mouse pos and updates keypresses
	Input::ResetMousePos(); 
	Input::CheckInputStatus();
}

void Scene::_render() {
	_renderer->render(_renderables, _lights, _mainCamera, _directionalLight, _skybox);
}