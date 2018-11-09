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
														   Texture::LoadTexture(Filepath::ModelPath + "cyborg/cyborg_normal.png"),
														   Texture::LoadTexture(Filepath::ModelPath + "cyborg/cyborg_emission.png"),
														   nullptr,
														   32.0f,
														   1.0f);

	//create components for each entity and fill with data
	CameraComponent* cameraComponent = new CameraComponent(glm::perspective(glm::radians(45.0f), (float)Window::ScreenWidth / (float)Window::ScreenHeight, 0.1f, 100.0f), 45.0f, 5.0f, 25.0f);
	LightComponent* spotLightComponent = new LightComponent(LightType::Spot);
	spotLightComponent->lightAmbient = glm::vec3(0.0f);
	spotLightComponent->lightDiffuse = glm::vec3(1.0f);
	spotLightComponent->lightSpecular = glm::vec3(1.0f);
	spotLightComponent->constantAttenuation = 1.0f;
	spotLightComponent->linearAttenuation = 0.09f;
	spotLightComponent->quadraticAttenuation = 0.032f;
	spotLightComponent->innerCutoff = glm::cos(glm::radians(12.5f));
	spotLightComponent->outerCutoff = glm::cos(glm::radians(15.0f));

	LightComponent* directionalLightComponent = new LightComponent(LightType::Directional);
	directionalLightComponent->lightAmbient = glm::vec3(0.2f);
	directionalLightComponent->lightDiffuse = glm::vec3(0.5f);
	directionalLightComponent->lightSpecular = glm::vec3(1.0f);
	directionalLightComponent->lightDirection = glm::vec3(-0.2f, -1.0f, -0.3f);

	RenderComponent* renderComponent = new RenderComponent(cyborgModel, textureMaterial);

	//add components to their respective nodes
	mainCamera->addComponent(cameraComponent);
	mainCamera->addComponent(spotLightComponent);
	directionalLight->addComponent(directionalLightComponent);
	cyborg->addComponent(renderComponent);

	//add nodes to the world
	_world->addChild(mainCamera);
	_world->addChild(directionalLight);
	_world->addChild(cyborg);

	std::cout << "Scene initialized" << std::endl;
}
