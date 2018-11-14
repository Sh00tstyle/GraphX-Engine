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
	Node* sphere = new Node(glm::vec3(2.0f, 1.0f, 0.0f), "sphere");

	//adjust transforms
	Transform* transform = cyborg->getTransform();
	transform->scale(glm::vec3(0.5f));

	transform = plane->getTransform();
	transform->scale(glm::vec3(4.0f, 1.0f, 4.0f));

	transform = sphere->getTransform();
	transform->scale(glm::vec3(0.2f));

	//load models
	Model* cyborgModel = Model::LoadModel(Filepath::ModelPath + "cyborg/cyborg.obj");
	Model* planeModel = Model::LoadModel(Filepath::ModelPath + "plane.obj");
	Model* sphereModel = Model::LoadModel(Filepath::ModelPath + "sphere_smooth.obj");

	//load textures
	Texture* cyborgDiffuse = Texture::LoadTexture(Filepath::ModelPath + "cyborg/cyborg_diffuse.png", TextureFilter::Repeat, true); //load diffuse textures in linear space
	Texture* cyborgSpecular = Texture::LoadTexture(Filepath::ModelPath + "cyborg/cyborg_specular.png");
	Texture* cyborgNormal = Texture::LoadTexture(Filepath::ModelPath + "cyborg/cyborg_normal.png");
	Texture* cyborgEmission = Texture::LoadTexture(Filepath::ModelPath + "cyborg/cyborg_emission.png", TextureFilter::Repeat, true); //load emission textures in linear space

	//create materials
	TextureMaterial* textureMaterial = new TextureMaterial(cyborgDiffuse, cyborgSpecular, cyborgNormal, cyborgEmission);

	ColorMaterial* colorMaterial = new ColorMaterial(glm::vec3(0.5f), glm::vec3(1.0f), glm::vec3(0.3f));
	ColorMaterial* sphereMaterial = new ColorMaterial(glm::vec3(1.5f, 1.5f, 0.0f), glm::vec3(1.5f, 1.5f, 0.0f), glm::vec3(1.5f, 1.5f, 0.0f));

	//load skybox
	std::vector<std::string> cubemapFaces{
		"ocean/right.jpg",
		"ocean/left.jpg",
		"ocean/top.jpg",
		"ocean/bottom.jpg",
		"ocean/front.jpg",
		"ocean/back.jpg",
	};

	Texture* skybox = Texture::LoadCubemap(cubemapFaces, true); //load skyboxed in linear space

	//create components for each node and fill with data
	CameraComponent* cameraComponent = new CameraComponent(glm::perspective(glm::radians(45.0f), (float)Window::ScreenWidth / (float)Window::ScreenHeight, 0.1f, 100.0f), 45.0f, 5.0f, 25.0f);
	LightComponent* spotLightComponent = new LightComponent(LightType::Spot);
	spotLightComponent->lightAmbient = glm::vec3(0.0f);
	spotLightComponent->lightDiffuse = glm::vec3(0.5f);
	spotLightComponent->lightSpecular = glm::vec3(0.8f);
	spotLightComponent->constantAttenuation = 1.0f;
	spotLightComponent->linearAttenuation = 0.09f;
	spotLightComponent->quadraticAttenuation = 0.032f;
	spotLightComponent->innerCutoff = glm::cos(glm::radians(15.0f));
	spotLightComponent->outerCutoff = glm::cos(glm::radians(20.0f));

	LightComponent* pointLightComponent = new LightComponent(LightType::Point);
	pointLightComponent->lightAmbient = glm::vec3(0.2f, 0.2f, 0.0f);
	pointLightComponent->lightDiffuse = glm::vec3(0.5f, 0.5f, 0.0f);
	pointLightComponent->lightSpecular = glm::vec3(0.8f);
	pointLightComponent->constantAttenuation = 1.0f;
	pointLightComponent->linearAttenuation = 0.09f;
	pointLightComponent->quadraticAttenuation = 0.032f;

	LightComponent* directionalLightComponent = new LightComponent(LightType::Directional);
	directionalLightComponent->lightAmbient = glm::vec3(0.5f);
	directionalLightComponent->lightDiffuse = glm::vec3(0.5f);
	directionalLightComponent->lightSpecular = glm::vec3(0.1f);
	directionalLightComponent->lightDirection = glm::vec3(1.0f, -2.0f, -0.5f);

	RenderComponent* cyborgRenderComponent = new RenderComponent(cyborgModel, textureMaterial);
	RenderComponent* planeRenderComponent = new RenderComponent(planeModel, colorMaterial);
	RenderComponent* sphereRenderComponent = new RenderComponent(sphereModel, sphereMaterial);

	//add components to their respective nodes
	mainCamera->addComponent(cameraComponent);
	//mainCamera->addComponent(spotLightComponent);
	directionalLight->addComponent(directionalLightComponent);
	cyborg->addComponent(cyborgRenderComponent);
	plane->addComponent(planeRenderComponent);
	sphere->addComponent(sphereRenderComponent);
	//sphere->addComponent(pointLightComponent);

	//add nodes to the world
	_world->addChild(mainCamera);
	_world->addChild(directionalLight);
	_world->addChild(cyborg);
	_world->addChild(plane);
	_world->addChild(sphere);

	//set main camera, (main) directional light and skybox
	_setMainCamera(mainCamera);
	_setDirectionalLight(directionalLight);
	_setSkybox(skybox);

	std::cout << "Scene initialized" << std::endl;
}
