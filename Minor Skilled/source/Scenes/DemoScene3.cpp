#include "DemoScene3.h"

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

DemoScene3::DemoScene3():Scene() {
}

DemoScene3::~DemoScene3() {
}

void DemoScene3::initializeScene(World* world, SceneManager* manager) {
	std::cout << "Initializing Scene" << std::endl;

	//create scene objects which represent graph nodes
	Node* mainCamera = new Node(glm::vec3(0.0f, 1.0f, 3.5f), "mainCamera");
	Node* directionalLight = new Node(glm::vec3(0.0f, 0.0f, 0.0f), "directionalLight");
	Node* pointLight = new Node(glm::vec3(2.0f, 1.0f, 0.0f), "pointLight");

	Node* ground = new Node(glm::vec3(0.0f, 0.0f, 0.0f), "ground");

	Node* cerberus = new Node(glm::vec3(0.0f, 1.0f, 1.0f), "cerberus");

	Node* gold = new Node(glm::vec3(-2.5f, 1.0, -1.5f), "gold");
	Node* fabric = new Node(glm::vec3(-1.5f, 1.0, -1.5f), "fabric");
	Node* plastic = new Node(glm::vec3(-0.5f, 1.0, -1.5f), "plastic");
	Node* rustedIron = new Node(glm::vec3(0.5f, 1.0, -1.5f), "rustedIron");
	Node* sand = new Node(glm::vec3(1.5f, 1.0, -1.5f), "sand");
	Node* wood = new Node(glm::vec3(2.5f, 1.0, -1.5f), "wood");

	//adjust transforms
	Transform* transform = pointLight->getTransform();
	transform->scale(glm::vec3(0.1f));

	transform = ground->getTransform();
	transform->scale(glm::vec3(5.0f, 1.0f, 5.0f));

	transform = cerberus->getTransform();
	transform->setEulerRotation(-90.0f, 90.0f, 0.0f);
	transform->scale(glm::vec3(0.007f));

	transform = gold->getTransform();
	transform->scale(glm::vec3(0.2f));

	transform = fabric->getTransform();
	transform->scale(glm::vec3(0.2f));

	transform = plastic->getTransform();
	transform->scale(glm::vec3(0.2f));

	transform = rustedIron->getTransform();
	transform->scale(glm::vec3(0.2f));

	transform = sand->getTransform();
	transform->scale(glm::vec3(0.2f));

	transform = wood->getTransform();
	transform->scale(glm::vec3(0.2f));

	//load models
	std::cout << "Loading models..." << std::endl;

	Model* planeModel = Model::LoadModel(Filepath::ModelPath + "plane.obj");
	Model* sphereModel = Model::LoadModel(Filepath::ModelPath + "sphere_smooth.obj");

	Model* cerberusModel = Model::LoadModel(Filepath::ModelPath + "cerberus/Cerberus_LP.fbx");

	//load textures
	std::cout << "Loading textures..." << std::endl;

	Texture* groundAlbedo = Texture::LoadTexture(Filepath::TexturePath + "pbr/metal/albedo.png", TextureFilter::Repeat, true); //load albedo textures in linear space
	Texture* groundNormal = Texture::LoadTexture(Filepath::TexturePath + "pbr/metal/normal.png", TextureFilter::Repeat);
	Texture* groundMetallic = Texture::LoadTexture(Filepath::TexturePath + "pbr/metal/metallic.png", TextureFilter::Repeat);
	Texture* groundRoughness = Texture::LoadTexture(Filepath::TexturePath + "pbr/metal/roughness.png", TextureFilter::Repeat);
	Texture* groundAo = Texture::LoadTexture(Filepath::TexturePath + "pbr/metal/ao.png", TextureFilter::Repeat);

	Texture* cerberusAlbedo = Texture::LoadTexture(Filepath::ModelPath + "cerberus/Cerberus_A.tga", TextureFilter::Repeat, true);
	Texture* cerberusNormal = Texture::LoadTexture(Filepath::ModelPath + "cerberus/Cerberus_N.tga", TextureFilter::Repeat);
	Texture* cerberusMetallic = Texture::LoadTexture(Filepath::ModelPath + "cerberus/Cerberus_M.tga", TextureFilter::Repeat);
	Texture* cerberusRoughness = Texture::LoadTexture(Filepath::ModelPath + "cerberus/Cerberus_R.tga", TextureFilter::Repeat);
	Texture* cerberusAo = Texture::LoadTexture(Filepath::ModelPath + "cerberus/Cerberus_AO.tga", TextureFilter::Repeat);

	Texture* goldAlbedo = Texture::LoadTexture(Filepath::TexturePath + "pbr/gold/albedo.png", TextureFilter::Repeat, true); //load albedo textures in linear space
	Texture* goldNormal = Texture::LoadTexture(Filepath::TexturePath + "pbr/gold/normal.png", TextureFilter::Repeat);
	Texture* goldMetallic = Texture::LoadTexture(Filepath::TexturePath + "pbr/gold/metallic.png", TextureFilter::Repeat);
	Texture* goldRoughness = Texture::LoadTexture(Filepath::TexturePath + "pbr/gold/roughness.png", TextureFilter::Repeat);
	Texture* goldAo = Texture::LoadTexture(Filepath::TexturePath + "pbr/gold/ao.png", TextureFilter::Repeat);

	Texture* fabricAlbedo = Texture::LoadTexture(Filepath::TexturePath + "pbr/fabric/albedo.png", TextureFilter::Repeat, true); //load albedo textures in linear space
	Texture* fabricNormal = Texture::LoadTexture(Filepath::TexturePath + "pbr/fabric/normal.png", TextureFilter::Repeat);
	Texture* fabricMetallic = Texture::LoadTexture(Filepath::TexturePath + "pbr/fabric/metallic.png", TextureFilter::Repeat);
	Texture* fabricRoughness = Texture::LoadTexture(Filepath::TexturePath + "pbr/fabric/roughness.png", TextureFilter::Repeat);
	Texture* fabricAo = Texture::LoadTexture(Filepath::TexturePath + "pbr/fabric/ao.png", TextureFilter::Repeat);

	Texture* plasticAlbedo = Texture::LoadTexture(Filepath::TexturePath + "pbr/plastic/albedo.png", TextureFilter::Repeat, true); //load albedo textures in linear space
	Texture* plasticNormal = Texture::LoadTexture(Filepath::TexturePath + "pbr/plastic/normal.png", TextureFilter::Repeat);
	Texture* plasticMetallic = Texture::LoadTexture(Filepath::TexturePath + "pbr/plastic/metallic.png", TextureFilter::Repeat);
	Texture* plasticRoughness = Texture::LoadTexture(Filepath::TexturePath + "pbr/plastic/roughness.png", TextureFilter::Repeat);
	Texture* plasticAo = Texture::LoadTexture(Filepath::TexturePath + "pbr/plastic/ao.png", TextureFilter::Repeat);

	Texture* rustedIronAlbedo = Texture::LoadTexture(Filepath::TexturePath + "pbr/rusted_iron/albedo.png", TextureFilter::Repeat, true); //load albedo textures in linear space
	Texture* rustedIronNormal = Texture::LoadTexture(Filepath::TexturePath + "pbr/rusted_iron/normal.png", TextureFilter::Repeat);
	Texture* rustedIronMetallic = Texture::LoadTexture(Filepath::TexturePath + "pbr/rusted_iron/metallic.png", TextureFilter::Repeat);
	Texture* rustedIronRoughness = Texture::LoadTexture(Filepath::TexturePath + "pbr/rusted_iron/roughness.png", TextureFilter::Repeat);
	Texture* rustedIronAo = Texture::LoadTexture(Filepath::TexturePath + "pbr/rusted_iron/ao.png", TextureFilter::Repeat);

	Texture* sandAlbedo = Texture::LoadTexture(Filepath::TexturePath + "pbr/sand/albedo.png", TextureFilter::Repeat, true); //load albedo textures in linear space
	Texture* sandNormal = Texture::LoadTexture(Filepath::TexturePath + "pbr/sand/normal.png", TextureFilter::Repeat);
	Texture* sandMetallic = Texture::LoadTexture(Filepath::TexturePath + "pbr/sand/metallic.png", TextureFilter::Repeat);
	Texture* sandRoughness = Texture::LoadTexture(Filepath::TexturePath + "pbr/sand/roughness.png", TextureFilter::Repeat);
	Texture* sandAo = Texture::LoadTexture(Filepath::TexturePath + "pbr/sand/ao.png", TextureFilter::Repeat);

	Texture* woodAlbedo = Texture::LoadTexture(Filepath::TexturePath + "pbr/wood/albedo.png", TextureFilter::Repeat, true); //load albedo textures in linear space
	Texture* woodNormal = Texture::LoadTexture(Filepath::TexturePath + "pbr/wood/normal.png", TextureFilter::Repeat);
	Texture* woodMetallic = Texture::LoadTexture(Filepath::TexturePath + "pbr/wood/metallic.png", TextureFilter::Repeat);
	Texture* woodRoughness = Texture::LoadTexture(Filepath::TexturePath + "pbr/wood/roughness.png", TextureFilter::Repeat);
	Texture* woodAo = Texture::LoadTexture(Filepath::TexturePath + "pbr/wood/ao.png", TextureFilter::Repeat);

	//load skybox
	std::cout << "Loading skybox..." << std::endl;

	Texture* skybox = Texture::LoadHDR(Filepath::SkyboxPath + "Hamarikyu_Bridge_B/14-Hamarikyu_Bridge_B_3k.hdr");

	//create materials
	ColorMaterial* pointLightMat = new ColorMaterial(glm::vec3(1.5f, 1.5f, 0.0f), glm::vec3(1.5f, 1.5f, 0.0f), 0.0f);

	PBRMaterial* groundMat = new PBRMaterial(groundAlbedo, groundNormal, groundMetallic, groundRoughness, groundAo, BlendMode::Opaque);

	PBRMaterial* cerberusMat = new PBRMaterial(cerberusAlbedo, cerberusNormal, cerberusMetallic, cerberusRoughness, cerberusAo, BlendMode::Opaque);

	PBRMaterial* goldMat = new PBRMaterial(goldAlbedo, goldNormal, goldMetallic, goldRoughness, goldAo, BlendMode::Opaque);
	PBRMaterial* fabricMat = new PBRMaterial(fabricAlbedo, fabricNormal, fabricMetallic, fabricRoughness, fabricAo, BlendMode::Opaque);
	PBRMaterial* plasticMat = new PBRMaterial(plasticAlbedo, plasticNormal, plasticMetallic, plasticRoughness, plasticAo, BlendMode::Opaque);
	PBRMaterial* rustedIronMat = new PBRMaterial(rustedIronAlbedo, rustedIronNormal, rustedIronMetallic, rustedIronRoughness, rustedIronAo, BlendMode::Opaque);
	PBRMaterial* sandMat = new PBRMaterial(sandAlbedo, sandNormal, sandMetallic, sandRoughness, sandAo, BlendMode::Opaque);
	PBRMaterial* woodMat = new PBRMaterial(woodAlbedo, woodNormal, woodMetallic, woodRoughness, woodAo, BlendMode::Opaque);

	//create components for each node and fill with data
	CameraComponent* cameraComponent = new CameraComponent(glm::perspective(glm::radians(45.0f), (float)Window::ScreenWidth / (float)Window::ScreenHeight, 0.1f, 100.0f), 45.0f, 0.1f, 100.0f, 5.0f, 25.0f);
	
	LightComponent* spotLightComponent = new LightComponent(LightType::Spot);
	spotLightComponent->lightAmbient = glm::vec3(0.1f);
	spotLightComponent->lightDiffuse = glm::vec3(0.5f, 0.0f, 0.0f);
	spotLightComponent->lightSpecular = glm::vec3(0.8f);
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

	LightComponent* directionalLightComponent = new LightComponent(LightType::Directional);
	directionalLightComponent->lightAmbient = glm::vec3(0.1f);
	directionalLightComponent->lightDiffuse = glm::vec3(0.5f);
	directionalLightComponent->lightSpecular = glm::vec3(0.1f);
	directionalLightComponent->lightDirection = glm::vec3(1.0f, -2.0f, -1.0f);

	RenderComponent* pointLightRenderComponent = new RenderComponent(sphereModel, pointLightMat);

	RenderComponent* groundRenderComponent = new RenderComponent(planeModel, groundMat);

	RenderComponent* cerberusRenderComponent = new RenderComponent(cerberusModel, cerberusMat);

	RenderComponent* goldRenderComponent = new RenderComponent(sphereModel, goldMat);
	RenderComponent* fabricRenderComponent = new RenderComponent(sphereModel, fabricMat);
	RenderComponent* plasticRenderComponent = new RenderComponent(sphereModel, plasticMat);
	RenderComponent* rustedIronRenderComponent = new RenderComponent(sphereModel, rustedIronMat);
	RenderComponent* sandRenderComponent = new RenderComponent(sphereModel, sandMat);
	RenderComponent* woodRenderComponent = new RenderComponent(sphereModel, woodMat);

	//add components to their respective nodes
	mainCamera->addComponent(cameraComponent);
	mainCamera->addComponent(spotLightComponent);

	directionalLight->addComponent(directionalLightComponent);

	pointLight->addComponent(pointLightRenderComponent);
	pointLight->addComponent(pointLightComponent);

	ground->addComponent(groundRenderComponent);

	cerberus->addComponent(cerberusRenderComponent);

	gold->addComponent(goldRenderComponent);
	fabric->addComponent(fabricRenderComponent);
	plastic->addComponent(plasticRenderComponent);
	rustedIron->addComponent(rustedIronRenderComponent);
	sand->addComponent(sandRenderComponent);
	wood->addComponent(woodRenderComponent);

	//add nodes to the world
	world->addChild(mainCamera);
	world->addChild(directionalLight);
	world->addChild(pointLight);

	world->addChild(ground);

	world->addChild(cerberus);

	world->addChild(gold);
	world->addChild(fabric);
	world->addChild(plastic);
	world->addChild(rustedIron);
	world->addChild(sand);
	world->addChild(wood);

	//set main camera, (main) directional light and skybox
	manager->setMainCamera(mainCamera);
	manager->setDirectionalLight(directionalLight);
	manager->setSkybox(skybox, true);

	std::cout << "Scene initialized" << std::endl;
}
