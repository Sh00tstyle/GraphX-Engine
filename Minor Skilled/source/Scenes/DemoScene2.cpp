#include "DemoScene2.h"

#include <iostream>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "../Engine/SceneManager.h"
#include "../Engine/Window.h"
#include "../Engine/Node.h"
#include "../Engine/World.h"
#include "../Engine/Component.h"
#include "../Engine/Texture.h"
#include "../Engine/Transform.h"
#include "../Engine/Model.h"
#include "../Engine/Renderer.h"

#include "../Components/CameraComponent.h"
#include "../Components/RenderComponent.h"
#include "../Components/LightComponent.h"

#include "../Materials/ColorMaterial.h"
#include "../Materials/TextureMaterial.h"
#include "../Materials/PBRMaterial.h"

#include "../Utility/Filepath.h"
#include "../Utility/LightType.h"
#include "../Utility/BlendMode.h"

DemoScene2::DemoScene2():Scene() {
}

DemoScene2::~DemoScene2() {
}

void DemoScene2::initializeScene(World* world, SceneManager* manager) {
	std::cout << "Initializing Scene" << std::endl;

	//create scene objects which represent graph nodes
	Node* mainCamera = new Node(glm::vec3(0.0f, 1.0f, 3.5f), "mainCamera");
	Node* pointLight = new Node(glm::vec3(0.0f, 5.8f, 0.0f), "pointLight");

	Node* cyborg = new Node(glm::vec3(0.0f, 0.0f, 0.0f), "cyborg");

	Node* ground = new Node(glm::vec3(0.0f, 0.0f, 0.0f), "ground");
	Node* leftWall = new Node(glm::vec3(-5.0f, 3.0f, 0.0f), "leftWall");
	Node* rightWall = new Node(glm::vec3(5.0f, 3.0f, 0.0f), "rightWall");
	Node* roof = new Node(glm::vec3(0.0f, 6.0f, 0.0f), "roof");

	Node* mirror = new Node(glm::vec3(2.0f, 1.0f, 2.5f), "mirror");
	Node* glass = new Node(glm::vec3(-3.0f, 0.5f, -2.0f), "glass");

	Node* windowFront = new Node(glm::vec3(0.0f, 3.0f, 5.0f), "windowFront");
	Node* windowBack = new Node(glm::vec3(0.0f, 3.0f, -5.0f), "windowBack");

	//adjust transforms
	Transform* transform = pointLight->getTransform();
	transform->scale(glm::vec3(0.4f));

	transform = cyborg->getTransform();
	transform->scale(glm::vec3(0.5f));

	transform = ground->getTransform();
	transform->scale(glm::vec3(5.0f, 1.0f, 5.0f));

	transform = leftWall->getTransform();
	transform->setEulerRotation(90.0f, 90.0f, 0.0f);
	transform->scale(glm::vec3(5.0f, 1.0f, 3.0f));

	transform = rightWall->getTransform();
	transform->setEulerRotation(90.0f, 270.0f, 0.0f);
	transform->scale(glm::vec3(5.0f, 1.0f, 3.0f));

	transform = roof->getTransform();
	transform->setEulerRotation(0.0f, 0.0f, 180.0f);
	transform->scale(glm::vec3(5.0f, 1.0f, 5.0f));

	transform = mirror->getTransform();
	transform->scale(glm::vec3(0.25f));

	transform = glass->getTransform();
	transform->scale(glm::vec3(0.25f));

	transform = windowFront->getTransform();
	transform->setEulerRotation(90.0f, 0.0f, 0.0f);
	transform->scale(glm::vec3(5.0f, 1.0f, 3.0f));

	transform = windowBack->getTransform();
	transform->setEulerRotation(90.0f, 0.0f, 0.0f);
	transform->scale(glm::vec3(5.0f, 1.0f, 3.0f));

	//load models
	std::cout << "Loading models..." << std::endl;

	Model* cyborgModel = Model::LoadModel(Filepath::ModelPath + "cyborg/cyborg.obj");
	Model* planeModel = Model::LoadModel(Filepath::ModelPath + "plane.obj");
	Model* sphereModel = Model::LoadModel(Filepath::ModelPath + "sphere_smooth.obj");
	Model* suzannaModel = Model::LoadModel(Filepath::ModelPath + "suzanna_smooth.obj");

	//load textures
	std::cout << "Loading textures..." << std::endl;

	Texture* cyborgDiffuse = Texture::LoadTexture(Filepath::ModelPath + "cyborg/cyborg_diffuse.png", TextureFilter::Repeat, true); //load diffuse textures in linear space
	Texture* cyborgSpecular = Texture::LoadTexture(Filepath::ModelPath + "cyborg/cyborg_specular.png", TextureFilter::Repeat);
	Texture* cyborgNormal = Texture::LoadTexture(Filepath::ModelPath + "cyborg/cyborg_normal.png", TextureFilter::Repeat);
	Texture* cyborgEmission = Texture::LoadTexture(Filepath::ModelPath + "cyborg/cyborg_emission.png", TextureFilter::Repeat);

	Texture* whiteTexture = Texture::LoadTexture(Filepath::TexturePath + "white.png", TextureFilter::Repeat);

	Texture* groundDiffuse = Texture::LoadTexture(Filepath::TexturePath + "wood.png", TextureFilter::Repeat, true);

	Texture* brickDiffuse = Texture::LoadTexture(Filepath::TexturePath + "brickwall.jpg", TextureFilter::Repeat, true);
	Texture* brickNormal = Texture::LoadTexture(Filepath::TexturePath + "brickwall_normal.jpg");

	Texture* window1Diffuse = Texture::LoadTexture(Filepath::TexturePath + "window.png", TextureFilter::Repeat, true);
	Texture* window2Diffuse = Texture::LoadTexture(Filepath::TexturePath + "window2.png", TextureFilter::Repeat, true);

	//load skybox
	std::cout << "Loading skybox..." << std::endl;

	Texture* skybox = Texture::LoadHDR(Filepath::SkyboxPath + "Milkyway/Milkyway_Small.hdr");

	//create materials
	ColorMaterial* pointLightMat = new ColorMaterial(glm::vec3(1.5f, 1.5f, 0.0f), glm::vec3(1.5f, 1.5f, 0.0f), 0.0f);

	TextureMaterial* cyborgMat = new TextureMaterial(cyborgDiffuse, cyborgSpecular, cyborgNormal, BlendMode::Opaque);
	cyborgMat->setEmissionMap(cyborgEmission);

	TextureMaterial* groundMat = new TextureMaterial(groundDiffuse, whiteTexture, nullptr, BlendMode::Opaque);
	groundMat->setShininess(64.0f);

	TextureMaterial* brickMat = new TextureMaterial(brickDiffuse, nullptr, brickNormal, BlendMode::Opaque);

	TextureMaterial* mirrorMat = new TextureMaterial(whiteTexture, BlendMode::Opaque);
	mirrorMat->setReflectionMap(whiteTexture);

	TextureMaterial* glassMat = new TextureMaterial(whiteTexture, BlendMode::Opaque);
	glassMat->setReflectionMap(whiteTexture);
	glassMat->setRefractionFactor(1.52f);

	TextureMaterial* window1Mat = new TextureMaterial(window1Diffuse, BlendMode::Transparent);
	TextureMaterial* window2Mat = new TextureMaterial(window2Diffuse, BlendMode::Transparent);

	//create components for each node and fill with data
	CameraComponent* cameraComponent = new CameraComponent(glm::perspective(glm::radians(45.0f), (float)Window::ScreenWidth / (float)Window::ScreenHeight, 0.1f, 100.0f), 45.0f, 0.1f, 100.0f, 5.0f, 25.0f);

	LightComponent* spotLightComponent = new LightComponent(LightType::Spot);
	spotLightComponent->lightAmbient = glm::vec3(1.0f);
	spotLightComponent->lightDiffuse = glm::vec3(1.0f);
	spotLightComponent->lightSpecular = glm::vec3(1.0f);
	spotLightComponent->constantAttenuation = 1.0f;
	spotLightComponent->linearAttenuation = 0.09f;
	spotLightComponent->quadraticAttenuation = 0.032f;
	spotLightComponent->innerCutoff = glm::cos(glm::radians(15.0f));
	spotLightComponent->outerCutoff = glm::cos(glm::radians(20.0f));

	LightComponent* pointLightComponent = new LightComponent(LightType::Point);
	pointLightComponent->lightAmbient = glm::vec3(0.1f);
	pointLightComponent->lightDiffuse = glm::vec3(0.5f, 0.5f, 0.0f);
	pointLightComponent->lightSpecular = glm::vec3(0.8f);
	pointLightComponent->constantAttenuation = 1.0f;
	pointLightComponent->linearAttenuation = 0.09f;
	pointLightComponent->quadraticAttenuation = 0.032f;

	LightComponent* point2LightComponent = new LightComponent(LightType::Point);
	point2LightComponent->lightAmbient = glm::vec3(0.1f);
	point2LightComponent->lightDiffuse = glm::vec3(0.0f, 0.0f, 0.5f);
	point2LightComponent->lightSpecular = glm::vec3(0.8f);
	point2LightComponent->constantAttenuation = 1.0f;
	point2LightComponent->linearAttenuation = 0.09f;
	point2LightComponent->quadraticAttenuation = 0.032f;

	RenderComponent* pointLightRenderComponent = new RenderComponent(sphereModel, pointLightMat);

	RenderComponent* cyborgRenderComponent = new RenderComponent(cyborgModel, cyborgMat);

	RenderComponent* planeRenderComponent = new RenderComponent(planeModel, groundMat);
	RenderComponent* leftWallRenderComponent = new RenderComponent(planeModel, brickMat);
	RenderComponent* rightWallRenderComponent = new RenderComponent(planeModel, brickMat);
	RenderComponent* roofRenderComponent = new RenderComponent(planeModel, brickMat);

	RenderComponent* mirrorRenderComponent = new RenderComponent(sphereModel, mirrorMat);
	RenderComponent* glassRenderComponent = new RenderComponent(suzannaModel, glassMat);

	RenderComponent* windowFrontRenderComponent = new RenderComponent(planeModel, window1Mat);
	RenderComponent* windowBackRenderComponent = new RenderComponent(planeModel, window2Mat);

	//add components to their respective nodes
	mainCamera->addComponent(cameraComponent);
	mainCamera->addComponent(spotLightComponent);

	pointLight->addComponent(pointLightRenderComponent);
	pointLight->addComponent(pointLightComponent);

	cyborg->addComponent(cyborgRenderComponent);
	ground->addComponent(planeRenderComponent);
	leftWall->addComponent(leftWallRenderComponent);
	rightWall->addComponent(rightWallRenderComponent);
	roof->addComponent(roofRenderComponent);

	mirror->addComponent(mirrorRenderComponent);
	mirror->addComponent(point2LightComponent);
	glass->addComponent(glassRenderComponent);

	windowFront->addComponent(windowFrontRenderComponent);
	windowBack->addComponent(windowBackRenderComponent);

	//add nodes to the world
	world->addChild(mainCamera);
	world->addChild(pointLight);

	world->addChild(cyborg);

	world->addChild(ground);
	world->addChild(leftWall);
	world->addChild(rightWall);
	world->addChild(roof);

	world->addChild(mirror);
	world->addChild(glass);

	world->addChild(windowFront);
	world->addChild(windowBack);

	//set main camera, (main) directional light and skybox
	manager->setMainCamera(mainCamera);
	manager->setSkybox(skybox, true);

	std::cout << "Scene initialized" << std::endl;
}
