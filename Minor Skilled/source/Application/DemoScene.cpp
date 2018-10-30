#include "DemoScene.h"

#include <iostream>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "../Engine/Window.h"
#include "../Engine/Node.h"
#include "../Engine/World.h"
#include "../Engine/Component.h"
#include "../Engine/Texture.h"
#include "../Engine/Transform.h"
#include "../Engine/Model.h"

#include "../Components/CameraComponent.h"
#include "../Components/RenderComponent.h"
#include "../Components/LightComponent.h"

#include "../Materials/ColorMaterial.h"
#include "../Materials/TextureMaterial.h"

#include "../Utility/Filepath.h"
#include "../Utility/LightType.h"

DemoScene::DemoScene():Scene() {
}

DemoScene::~DemoScene() {
}

void DemoScene::_initializeScene() {
	std::cout << "Initializing Scene" << std::endl;

	//create scene objects which represent graph nodes
	Node* mainCamera = new Node(glm::vec3(0.0f, 0.5f, 3.0f), "mainCamera");
	Node* directionalLight = new Node(glm::vec3(0.0f, 0.0f, 0.0f), "directionalLight");
	Node* cyborg = new Node(glm::vec3(0.0f, 0.0f, 0.0f), "cyborg");

	//adjust transforms
	Transform* cyborgTransform = cyborg->getTransform();
	cyborgTransform->localTransform = glm::scale(cyborgTransform->localTransform, glm::vec3(0.5f));

	//load models
	Model* cyborgModel = Model::LoadModel(Filepath::ModelPath + "cyborg/cyborg.obj");

	//create materials
	TextureMaterial* textureMaterial = new TextureMaterial(Texture::LoadTexture(Filepath::ModelPath + "cyborg/cyborg_diffuse.png"),
														  Texture::LoadTexture(Filepath::ModelPath + "cyborg/cyborg_specular.png"),
														  nullptr,
														  nullptr,
														  32.0f);

	//create components for each entity and fill with data
	CameraComponent* cameraComponent = new CameraComponent(glm::perspective(glm::radians(45.0f), (float)Window::ScreenWidth / (float)Window::ScreenHeight, 0.1f, 100.0f),
														   45.0f,
														   5.0f,
														   25.0f);
	LightComponent* lightComponent = new LightComponent(LightType::Directional);
	RenderComponent* renderComponent = new RenderComponent(cyborgModel, textureMaterial);

	lightComponent->lightAmbient = glm::vec3(0.2f);
	lightComponent->lightDiffuse = glm::vec3(0.5f);
	lightComponent->lightSpecular = glm::vec3(1.0f);
	lightComponent->lightDirection = glm::vec3(1.0f, 0.0f, 0.0f);

	//add components to their respective nodes
	mainCamera->addComponent(cameraComponent);
	directionalLight->addComponent(lightComponent);
	cyborg->addComponent(renderComponent);

	//add nodes to the world
	_world->addChild(mainCamera);
	_world->addChild(directionalLight);
	_world->addChild(cyborg);

	std::cout << "Scene initialized" << std::endl;
}
