#include "SceneManager.h"

#include <iostream>
#include <string>
#include <bitset>

#define STB_IMAGE_IMPLEMENTATION //NOTE: has to be done once in the project BEFORE including std_image.h
#include "../../dependencies/stb_image/stb_image.h"

#include "../Engine/Scene.h"
#include "../Engine/Node.h"
#include "../Engine/Window.h"
#include "../Engine/World.h"
#include "../Engine/Renderer.h"
#include "../Engine/Texture.h"
#include "../Engine/Debug.h"

#include "../UI/OverlayUI.h"

#include "../Components/LightComponent.h"

#include "../Utility/Time.h"
#include "../Utility/Input.h"
#include "../Utility/ComponentType.h"
#include "../Utility/LightType.h"

SceneManager::SceneManager(): _queuedSceneIndex(-1) {
}

SceneManager::~SceneManager() {
	delete _window;
	delete _world;
	delete _skybox;
	delete _renderer;
	delete _profiler;
	delete _ui;

	for(unsigned int i = 0; i < _scenes.size(); i++) {
		delete _scenes[i];
	}
}

void SceneManager::queueScene(int index) {
	_queuedSceneIndex = index;
}

void SceneManager::addScene(Scene* scene) {
	_scenes.push_back(scene);
}

void SceneManager::setSkybox(Texture* skybox, bool isEquirectangular) {
	if(skybox == nullptr) {
		std::cout << "ERROR: Unable to assign skybox. It was null." << std::endl;
		return;
	}

	if(isEquirectangular) _skybox = _renderer->convertEquiToCube(skybox);
	else _skybox = skybox;
}

void SceneManager::setMainCamera(Node* mainCamera) {
	if(!mainCamera->hasComponent(ComponentType::Camera)) {
		std::cout << "ERROR: Unable to assign main camera. It has no camera component." << std::endl;
		return;
	}

	_mainCamera = mainCamera;
}

void SceneManager::setDirectionalLight(Node * directionalLight) {
	if(!directionalLight->hasComponent(ComponentType::Light)) {
		std::cout << "ERROR: Unable to assign directional light. It has no light component." << std::endl;
		return;
	} else if(((LightComponent*)directionalLight->getComponent(ComponentType::Light))->lightType != LightType::Directional) {
		std::cout << "ERROR: Unable to assign directional light. Its light type is not DIRECTIONAL." << std::endl;
		return;
	}

	_directionalLight = directionalLight;
}

void SceneManager::initialize(unsigned int sceneIndex) {
	std::cout << "---Initializing Engine---" << std::endl;

	_window = new Window(1920, 1017, 0, 27, "GraphX Engine v1.0");
	_world = new World(); //scene graph
	_profiler = new Debug();
	_renderer = new Renderer(_profiler);
	_ui = new OverlayUI(this, _window, _profiler);

	_skybox = nullptr;
	_mainCamera = nullptr;
	_directionalLight = nullptr;

	_queuedSceneIndex = sceneIndex;
	_loadScene();
	
	std::cout << "---Engine initialized---" << std::endl;

	//initialization info
	Debug::Log("Engine initialized");
	Debug::Log("Hold the right mouse button to control the camera");
	Debug::Log("Use W/A/S/D to move");
	Debug::Log("Use Space/LShift to ascend/descend");
	Debug::Log("Use RShift to reset the camera");
	Debug::Log("Use Enter to toggle the UI");
	Debug::Log("Use Escape to close the application");
}

void SceneManager::run() {
	while(_window->isOpen()) { //render loop
		Time::UpdateTime();

		Input::ProcessInput();

		_update();
		_render();

		_profiler->profile();

		_window->swapBuffers();
		_window->pollEvents();

		if(_queuedSceneIndex != -1) _loadScene();
	}
}

void SceneManager::_update() {
	_profiler->startQuery(QueryType::Update);

	//clear vectors
	_renderables.clear();
	_lights.clear();

	//update scene graph and pass in vectors to fill
	_world->update(_renderables, _lights); 

	//resets the last mouse pos back to the current mouse pos and updates keypresses
	Input::ResetMousePos(); 
	Input::CheckInputStatus();

	_profiler->endQuery(QueryType::Update);
}

void SceneManager::_render() {
	//start new imgui frame and setup the UI after the update is done
	_ui->setupFrame(_world);

	//render the scene
	_renderer->render(_renderables, _lights, _mainCamera, _directionalLight, _skybox);

	//render the ui on top of the scene
	_profiler->startQuery(QueryType::UI);
	_ui->render();
	_profiler->endQuery(QueryType::UI);
}

void SceneManager::_loadScene() {
	//unload current scene objects and the skybox
	_world->erase();

	delete _skybox;

	//set unused members to nullptr
	_skybox = nullptr;
	_mainCamera = nullptr;
	_directionalLight = nullptr;

	_renderables.clear();
	_lights.clear();

	//load new scene
	std::cout << "Loading Scene " + std::to_string(_queuedSceneIndex + 1) + "..." << std::endl;

	_scenes[_queuedSceneIndex]->initializeScene(_world, this);

	//render environment maps for the scene
	_initializeEnvironmentMaps();

	std::cout << "Scene " + std::to_string(_queuedSceneIndex + 1) + " successfully loaded" << std::endl;

	//reset queued scene index
	_queuedSceneIndex = -1;
}

void SceneManager::_initializeEnvironmentMaps() {
	//update the scene graph first to get the correct model matrices and data
	_update();

	_profiler->startQuery(QueryType::Environment);

	//render the environment maps before the renderloop starts
	_renderer->renderEnvironmentMaps(_renderables, _directionalLight, _skybox);

	_profiler->endQuery(QueryType::Environment);
}
