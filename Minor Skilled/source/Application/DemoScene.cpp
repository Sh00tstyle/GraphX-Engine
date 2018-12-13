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
	Node* sphereReflect = new Node(glm::vec3(2.0f, 0.8f, -1.0f), "sphereReflect");
	Node* sphereLight = new Node(glm::vec3(-2.0f, 1.0f, 0.0f), "sphereLight");
	//Node* cube = new Node(glm::vec3(2.0f, 0.3f, 1.0f), "cube");
	Node* cube = new Node(glm::vec3(0.0f, -1.0f, 0.0f), "cube");
	Node* glass = new Node(glm::vec3(3.0f, 0.3f, 3.0f), "glass");
	Node* bricks = new Node(glm::vec3(-2.5f, 0.5f, 2.5f), "bricks");
	Node* pbrSphere = new Node(glm::vec3(-2.5f, 0.5, -1.5f), "pbrSphere");

	//adjust transforms
	Transform* transform = cyborg->getTransform();
	transform->scale(glm::vec3(0.5f));

	transform = plane->getTransform();
	transform->scale(glm::vec3(4.0f, 1.0f, 4.0f));

	transform = sphereReflect->getTransform();
	transform->scale(glm::vec3(0.2f));

	transform = sphereLight->getTransform();
	transform->scale(glm::vec3(0.2f));

	transform = cube->getTransform();
	transform->scale(glm::vec3(0.3f));

	transform = glass->getTransform();
	transform->scale(glm::vec3(0.3f));
	transform->rotate(90.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	transform = bricks->getTransform();
	transform->scale(glm::vec3(0.3f));
	transform->rotate(45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	transform->rotate(45.0f, glm::vec3(0.0f, 0.0f, 1.0f));

	transform = pbrSphere->getTransform();
	transform->scale(glm::vec3(0.2f));

	//load models
	std::cout << "Loading models..." << std::endl;

	Model* cyborgModel = Model::LoadModel(Filepath::ModelPath + "cyborg/cyborg.obj");
	Model* planeModel = Model::LoadModel(Filepath::ModelPath + "plane.obj");
	Model* sphereModel = Model::LoadModel(Filepath::ModelPath + "sphere_smooth.obj");
	Model* cubeModel = Model::LoadModel(Filepath::ModelPath + "cube_smooth.obj");

	//load textures
	std::cout << "Loading textures..." << std::endl;

	Texture* cyborgDiffuse = Texture::LoadTexture(Filepath::ModelPath + "cyborg/cyborg_diffuse.png", TextureFilter::Repeat, true); //load diffuse textures in linear space
	Texture* cyborgSpecular = Texture::LoadTexture(Filepath::ModelPath + "cyborg/cyborg_specular.png");
	Texture* cyborgNormal = Texture::LoadTexture(Filepath::ModelPath + "cyborg/cyborg_normal.png");
	Texture* cyborgEmission = Texture::LoadTexture(Filepath::ModelPath + "cyborg/cyborg_emission.png", TextureFilter::Repeat, true); //load emission textures in linear space

	Texture* reflectionMap = Texture::LoadTexture(Filepath::TexturePath + "reflection.png");
	Texture* blendTexture = Texture::LoadTexture(Filepath::TexturePath + "window.png", TextureFilter::Repeat, true); //load diffuse textures in linear space
	Texture* brickTexture = Texture::LoadTexture(Filepath::TexturePath + "bricks2.jpg", TextureFilter::Repeat, true); //load diffuse textures in linear space
	Texture* brickNormal = Texture::LoadTexture(Filepath::TexturePath + "bricks2_normal.jpg", TextureFilter::Repeat);
	Texture* heightTexture = Texture::LoadTexture(Filepath::TexturePath + "bricks2_disp.jpg", TextureFilter::Repeat);

	Texture* albedo = Texture::LoadTexture(Filepath::TexturePath + "pbr/gold/albedo.png", TextureFilter::Repeat, true); //load albedo textures in linear space
	Texture* normal = Texture::LoadTexture(Filepath::TexturePath + "pbr/gold/normal.png", TextureFilter::Repeat);
	Texture* metallic = Texture::LoadTexture(Filepath::TexturePath + "pbr/gold/metallic.png", TextureFilter::Repeat);
	Texture* roughness = Texture::LoadTexture(Filepath::TexturePath + "pbr/gold/roughness.png", TextureFilter::Repeat);
	Texture* ao = Texture::LoadTexture(Filepath::TexturePath + "pbr/gold/ao.png", TextureFilter::Repeat);

	//load skybox
	std::cout << "Loading skybox..." << std::endl;

	/**
	std::vector<std::string> cubemapFaces{
		"ocean/right.jpg",
		"ocean/left.jpg",
		"ocean/top.jpg",
		"ocean/bottom.jpg",
		"ocean/front.jpg",
		"ocean/back.jpg",
	};

	Texture* skybox = Texture::LoadCubemap(cubemapFaces, true); //load skyboxes in linear space
	/**/

	Texture* skybox = Texture::LoadHDR(Filepath::SkyboxPath + "Milkyway/Milkyway_Small.hdr"); //low res
	//Texture* skybox = Texture::LoadHDR(Filepath::SkyboxPath + "Milkyway/Milkyway_BG.jpg"); //high res

	//create materials
	TextureMaterial* textureMaterial = new TextureMaterial(cyborgDiffuse, cyborgSpecular, cyborgNormal, BlendMode::Opaque);
	textureMaterial->setEmissionMap(cyborgEmission);

	TextureMaterial* reflectionMaterial = new TextureMaterial(reflectionMap, BlendMode::Opaque);
	reflectionMaterial->setReflectionMap(reflectionMap);
	reflectionMaterial->setRefractionFactor(1.52f);

	TextureMaterial* blendMaterial = new TextureMaterial(blendTexture, BlendMode::Opaque);
	blendMaterial->setBlendMode(BlendMode::Transparent);

	TextureMaterial* heightMaterial = new TextureMaterial(brickTexture, BlendMode::Opaque);
	heightMaterial->setNormalMap(brickNormal);
	heightMaterial->setHeightMap(heightTexture);
	heightMaterial->setHeightScale(0.15f);

	ColorMaterial* colorMaterial = new ColorMaterial(glm::vec3(0.1f), glm::vec3(0.5f), glm::vec3(0.3f), 32.0f);
	ColorMaterial* sphereMaterial = new ColorMaterial(glm::vec3(1.5f, 1.5f, 0.0f), glm::vec3(1.5f, 1.5f, 0.0f), glm::vec3(1.5f, 1.5f, 0.0f));

	PBRMaterial* pbrMaterial = new PBRMaterial(albedo, normal, metallic, roughness, ao, BlendMode::Opaque);
	//pbrMaterial->setRefractionFactor(1.52f);

	//create components for each node and fill with data
	RenderComponent* cyborgRenderComponent = new RenderComponent(cyborgModel, textureMaterial);
	RenderComponent* planeRenderComponent = new RenderComponent(planeModel, colorMaterial);
	RenderComponent* sphereRenderComponent = new RenderComponent(sphereModel, reflectionMaterial);
	RenderComponent* sphereLightRenderComponent = new RenderComponent(sphereModel, sphereMaterial);
	RenderComponent* cubeRenderComponent = new RenderComponent(cubeModel, colorMaterial);
	RenderComponent* glassRenderComponent = new RenderComponent(planeModel, blendMaterial);
	RenderComponent* brickRenderComponent = new RenderComponent(planeModel, heightMaterial);
	RenderComponent* pbrRenderComponent = new RenderComponent(sphereModel, pbrMaterial);

	CameraComponent* cameraComponent = new CameraComponent(glm::perspective(glm::radians(45.0f), (float)Window::ScreenWidth / (float)Window::ScreenHeight, 0.1f, 100.0f), 45.0f, 0.1f, 100.0f, 5.0f, 25.0f);
	LightComponent* spotLightComponent = new LightComponent(LightType::Spot);
	spotLightComponent->lightAmbient = glm::vec3(0.1f);
	spotLightComponent->lightDiffuse = glm::vec3(0.5f, 0.0f, 0.0f);
	//spotLightComponent->lightDiffuse = glm::vec3(1.0f);
	spotLightComponent->lightSpecular = glm::vec3(0.8f);
	spotLightComponent->constantAttenuation = 1.0f;
	spotLightComponent->linearAttenuation = 0.09f;
	spotLightComponent->quadraticAttenuation = 0.032f;
	spotLightComponent->innerCutoff = glm::cos(glm::radians(15.0f));
	spotLightComponent->outerCutoff = glm::cos(glm::radians(20.0f));

	LightComponent* pointLightComponent = new LightComponent(LightType::Point);
	pointLightComponent->lightAmbient = glm::vec3(0.1f);
	pointLightComponent->lightDiffuse = glm::vec3(0.5f, 0.5f, 0.0f);
	//pointLightComponent->lightDiffuse = glm::vec3(1.0f);
	pointLightComponent->lightSpecular = glm::vec3(0.8f);
	pointLightComponent->constantAttenuation = 1.0f;
	pointLightComponent->linearAttenuation = 0.09f;
	pointLightComponent->quadraticAttenuation = 0.032f;

	LightComponent* directionalLightComponent = new LightComponent(LightType::Directional);
	directionalLightComponent->lightAmbient = glm::vec3(0.1f);
	directionalLightComponent->lightDiffuse = glm::vec3(0.5f);
	directionalLightComponent->lightSpecular = glm::vec3(0.1f);
	directionalLightComponent->lightDirection = glm::vec3(1.0f, -2.0f, -1.0f);

	//add components to their respective nodes
	mainCamera->addComponent(cameraComponent);
	mainCamera->addComponent(spotLightComponent);
	directionalLight->addComponent(directionalLightComponent);
	sphereLight->addComponent(sphereLightRenderComponent);
	sphereLight->addComponent(pointLightComponent);

	cyborg->addComponent(cyborgRenderComponent);
	plane->addComponent(planeRenderComponent);
	sphereReflect->addComponent(sphereRenderComponent);
	cube->addComponent(cubeRenderComponent);
	glass->addComponent(glassRenderComponent);
	bricks->addComponent(brickRenderComponent);
	pbrSphere->addComponent(pbrRenderComponent);

	//set children
	//mainCamera->addChild(cube);

	//add nodes to the world
	_world->addChild(mainCamera);
	_world->addChild(directionalLight);
	_world->addChild(cyborg);
	_world->addChild(plane);
	_world->addChild(sphereReflect);
	_world->addChild(sphereLight);
	_world->addChild(cube);
	_world->addChild(glass);
	_world->addChild(bricks);
	_world->addChild(pbrSphere);

	//set main camera, (main) directional light and skybox
	_setMainCamera(mainCamera);
	_setDirectionalLight(directionalLight);
	_setSkybox(skybox, true);

	std::cout << "Scene initialized" << std::endl;
}
