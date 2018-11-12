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
#include "../Utility/BlendMode.h"

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
	Node* plane = new Node(glm::vec3(0.0f, -0.01f, 0.0f), "plane");

	//adjust transforms
	Transform* transform = cyborg->getTransform();
	transform->localTransform = glm::scale(transform->localTransform, glm::vec3(0.5f));

	transform = plane->getTransform();
	transform->localTransform = glm::scale(transform->localTransform, glm::vec3(4.0f, 1.0f, 4.0f));

	//load models
	Model* cyborgModel = Model::LoadModel(Filepath::ModelPath + "cyborg/cyborg.obj");
	Model* planeModel = Model::LoadModel(Filepath::ModelPath + "plane.obj");

	//create materials
	TextureMaterial* textureMaterial = new TextureMaterial(Texture::LoadTexture(Filepath::ModelPath + "cyborg/cyborg_diffuse.png"),
														   Texture::LoadTexture(Filepath::ModelPath + "cyborg/cyborg_specular.png"),
														   Texture::LoadTexture(Filepath::ModelPath + "cyborg/cyborg_normal.png"),
														   Texture::LoadTexture(Filepath::ModelPath + "cyborg/cyborg_emission.png"),
														   nullptr,
														   32.0f,
														   1.0f,
														   BlendMode::Opaque);

	ColorMaterial* colorMaterial = new ColorMaterial(glm::vec3(0.5f), 
													 glm::vec3(1.0f), 
													 glm::vec3(0.3f), 
													 32.0f);

	//load skybox
	std::vector<std::string> cubemapFaces{
		"ocean/right.jpg",
		"ocean/left.jpg",
		"ocean/top.jpg",
		"ocean/bottom.jpg",
		"ocean/front.jpg",
		"ocean/back.jpg",
	};

	Texture* skybox = Texture::LoadCubemap(cubemapFaces);

	//create components for each node and fill with data
	CameraComponent* cameraComponent = new CameraComponent(glm::perspective(glm::radians(45.0f), (float)Window::ScreenWidth / (float)Window::ScreenHeight, 0.1f, 100.0f), 45.0f, 5.0f, 25.0f);
	LightComponent* spotLightComponent = new LightComponent(LightType::Spot);
	spotLightComponent->lightAmbient = glm::vec3(0.0f);
	spotLightComponent->lightDiffuse = glm::vec3(1.0f);
	spotLightComponent->lightSpecular = glm::vec3(1.0f);
	spotLightComponent->constantAttenuation = 1.0f;
	spotLightComponent->linearAttenuation = 0.09f;
	spotLightComponent->quadraticAttenuation = 0.032f;
	spotLightComponent->innerCutoff = glm::cos(glm::radians(15.0f));
	spotLightComponent->outerCutoff = glm::cos(glm::radians(20.0f));

	LightComponent* directionalLightComponent = new LightComponent(LightType::Directional);
	directionalLightComponent->lightAmbient = glm::vec3(0.2f);
	directionalLightComponent->lightDiffuse = glm::vec3(0.5f);
	directionalLightComponent->lightSpecular = glm::vec3(1.0f);
	directionalLightComponent->lightDirection = glm::vec3(1.0f, -2.0f, -0.5f);

	RenderComponent* cyborgRenderComponent = new RenderComponent(cyborgModel, textureMaterial);

	RenderComponent* planeRenderComponent = new RenderComponent(planeModel, colorMaterial);

	//add components to their respective nodes
	mainCamera->addComponent(cameraComponent);
	mainCamera->addComponent(spotLightComponent);
	directionalLight->addComponent(directionalLightComponent);
	cyborg->addComponent(cyborgRenderComponent);
	plane->addComponent(planeRenderComponent);

	//add nodes to the world
	_world->addChild(mainCamera);
	_world->addChild(directionalLight);
	_world->addChild(cyborg);
	_world->addChild(plane);

	//set main camera, (main) directional light and skybox
	_setMainCamera(mainCamera);
	_setDirectionalLight(directionalLight);
	_setSkybox(skybox);

	std::cout << "Scene initialized" << std::endl;
}
