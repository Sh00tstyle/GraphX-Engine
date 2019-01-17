#include "DemoScene1.h"

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

DemoScene1::DemoScene1():Scene() {
}

DemoScene1::~DemoScene1() {
}

void DemoScene1::initializeScene(World* world, SceneManager* manager) {
	std::cout << "Initializing Scene" << std::endl;

	//create scene objects which represent graph nodes
	Node* mainCamera = new Node(glm::vec3(0.2f, 2.5f, 7.0f), "mainCamera");
	Node* directionalLight = new Node(glm::vec3(0.0f, 0.0f, 0.0f), "directionalLight");

	Node* ground = new Node(glm::vec3(0.0f), "ground");

	Node* nyra = new Node(glm::vec3(0.0f, 0.17f, 0.0f), "nyra");
	Node* head = new Node(glm::vec3(0.0f), "head");
	Node* body = new Node(glm::vec3(0.0f), "body");

	Node* fern1 = new Node(glm::vec3(5.5f, 0.0f, 4.0f), "fern1");
	Node* fern2 = new Node(glm::vec3(1.0f, 0.0f, -5.0f), "fern2");
	Node* fern3 = new Node(glm::vec3(1.5f, 0.0f, 6.0f), "fern3");

	Node* grass1 = new Node(glm::vec3(-5.0f, 0.0f, 2.0f), "grass1");
	Node* grass2 = new Node(glm::vec3(6.0f, 0.0f, -1.0f), "grass2");
	Node* grass3 = new Node(glm::vec3(-0.2f, 0.0f, -3.5f), "grass3");
	Node* grass4 = new Node(glm::vec3(3.0f, 0.0f, 2.0f), "grass4");
	Node* grass5 = new Node(glm::vec3(-2.0f, 0.0f, -2.0f), "grass5");
	Node* grass6 = new Node(glm::vec3(0.0f, 0.0f, 6.0f), "grass6");

	Node* rock = new Node(glm::vec3(4.5f, 0.0f, -3.5f), "rock");
	Node* bench = new Node(glm::vec3(-5.0f, 0.0f, 0.0f), "bench");
	Node* stump = new Node(glm::vec3(3.5f, 0.0f, 6.0f), "stump");
	Node* log = new Node(glm::vec3(-3.5f, 0.0f, 5.0f), "log");

	Node* bricks = new Node(glm::vec3(-3.0f, 1.0f, -5.0f), "bricks");

	//adjust transforms
	Transform* transform = mainCamera->getTransform();
	transform->setEulerRotation(-10.0f, 0.0f, 0.0f);

	transform = nyra->getTransform();
	transform->scale(glm::vec3(0.1f));

	transform = fern1->getTransform();
	transform->scale(glm::vec3(0.015f));

	transform = fern2->getTransform();
	transform->scale(glm::vec3(0.01f));
	transform->setEulerRotation(0.0f, 45.0f, 0.0f);

	transform = fern3->getTransform();
	transform->scale(glm::vec3(0.01f));
	transform->setEulerRotation(0.0f, 225.0f, 0.0f);

	transform = grass1->getTransform();
	transform->scale(glm::vec3(0.01f));

	transform = grass2->getTransform();
	transform->scale(glm::vec3(0.01f));
	transform->setEulerRotation(0.0f, 45.0f, 0.0f);

	transform = grass3->getTransform();
	transform->scale(glm::vec3(0.01f));
	transform->setEulerRotation(0.0f, 225.0f, 0.0f);

	transform = grass4->getTransform();
	transform->scale(glm::vec3(0.01f));

	transform = grass5->getTransform();
	transform->scale(glm::vec3(0.01f));
	transform->setEulerRotation(0.0f, 45.0f, 0.0f);

	transform = grass6->getTransform();
	transform->scale(glm::vec3(0.01f));
	transform->setEulerRotation(0.0f, 225.0f, 0.0f);

	transform = rock->getTransform();
	transform->scale(glm::vec3(0.8f));

	transform = bench->getTransform();
	transform->scale(glm::vec3(0.012f));

	transform = stump->getTransform();
	transform->scale(glm::vec3(0.03f));

	transform = log->getTransform();
	transform->scale(glm::vec3(0.01f));
	transform->setEulerRotation(0.0f, 45.0f, 0.0f);

	transform = bricks->getTransform();
	transform->setEulerRotation(90.0f, 0.0f, 0.0f);

	//load models
	std::cout << "Loading models..." << std::endl;

	Model* planeModel = Model::LoadModel(Filepath::ModelPath + "plane.obj");

	Model* nyraHead = Model::LoadModel(Filepath::ModelPath + "nyra/nyra head.obj");
	Model* nyraBody = Model::LoadModel(Filepath::ModelPath + "nyra/nyra body.obj");

	Model* fernModel = Model::LoadModel(Filepath::ModelPath + "plants/FernBranch001.obj");
	Model* grassModel1 = Model::LoadModel(Filepath::ModelPath + "plants/GrassBlade001.obj");
	Model* grassModel2 = Model::LoadModel(Filepath::ModelPath + "plants/GrassBlade002.obj");

	Model* rockModel = Model::LoadModel(Filepath::ModelPath + "rock/rock.obj");
	Model* benchModel = Model::LoadModel(Filepath::ModelPath + "bench/bench.obj");
	Model* stumpModel = Model::LoadModel(Filepath::ModelPath + "stump/stump.obj");
	Model* logModel = Model::LoadModel(Filepath::ModelPath + "log/log.obj");

	//load textures
	std::cout << "Loading textures..." << std::endl;

	Texture* planeDiffuse = Texture::LoadTexture(Filepath::TexturePath + "pbr/grass/albedo.png", TextureFilter::Repeat, true);
	Texture* planeNormal = Texture::LoadTexture(Filepath::TexturePath + "pbr/grass/normal.png", TextureFilter::Repeat);

	Texture* headDiffuse = Texture::LoadTexture(Filepath::ModelPath + "nyra/head_d.png", TextureFilter::Repeat, true);
	Texture* headSpecular = Texture::LoadTexture(Filepath::ModelPath + "nyra/head_s.png", TextureFilter::Repeat);
	Texture* headNormal = Texture::LoadTexture(Filepath::ModelPath + "nyra/head_n.png", TextureFilter::Repeat);

	Texture* bodyDiffuse = Texture::LoadTexture(Filepath::ModelPath + "nyra/body_d.png", TextureFilter::Repeat, true);
	Texture* bodySpecular = Texture::LoadTexture(Filepath::ModelPath + "nyra/body_s.png", TextureFilter::Repeat);
	Texture* bodyNormal = Texture::LoadTexture(Filepath::ModelPath + "nyra/body_n.png", TextureFilter::Repeat);

	Texture* fernDiffuse = Texture::LoadTexture(Filepath::ModelPath + "plants/FernBranch001_COL_1K.png", TextureFilter::Repeat, true);
	Texture* grass1Diffuse = Texture::LoadTexture(Filepath::ModelPath + "plants/GrassBlades001_COL_1K.png", TextureFilter::Repeat, true);
	Texture* grass2Diffuse = Texture::LoadTexture(Filepath::ModelPath + "plants/GrassBlades002_COL_1K.png", TextureFilter::Repeat, true);

	Texture* rockDiffuse = Texture::LoadTexture(Filepath::ModelPath + "rock/Rock-Texture-Surface.jpg", TextureFilter::Repeat, true);

	Texture* benchDiffuse = Texture::LoadTexture(Filepath::ModelPath + "bench/MexicanDoors_BaseColor.png", TextureFilter::Repeat, true);
	Texture* benchSpecular = Texture::LoadTexture(Filepath::ModelPath + "bench/MexicanDoors_Glossiness.png");
	Texture* benchNormal = Texture::LoadTexture(Filepath::ModelPath + "bench/MexicanDoors_Normal.png");

	Texture* stumpDiffuse = Texture::LoadTexture(Filepath::ModelPath + "stump/Birch stump_D.png", TextureFilter::Repeat, true);
	Texture* stumpNormal = Texture::LoadTexture(Filepath::ModelPath + "stump/Birch stump_N.png");

	Texture* logDiffuse = Texture::LoadTexture(Filepath::ModelPath + "log/log_diffuse.png", TextureFilter::Repeat, true);
	Texture* logNormal = Texture::LoadTexture(Filepath::ModelPath + "log/log_normal.png");

	Texture* brickDiffuse = Texture::LoadTexture(Filepath::TexturePath + "bricks2.jpg", TextureFilter::Repeat, true);
	Texture* brickNormal = Texture::LoadTexture(Filepath::TexturePath + "bricks2_normal.jpg");
	Texture* brickHeight = Texture::LoadTexture(Filepath::TexturePath + "bricks2_disp.jpg");

	//load skybox
	std::cout << "Loading skybox..." << std::endl;

	std::vector<std::string> cubemapFaces{
		"ocean/right.jpg",
		"ocean/left.jpg",
		"ocean/top.jpg",
		"ocean/bottom.jpg",
		"ocean/front.jpg",
		"ocean/back.jpg",
	};

	Texture* skybox = Texture::LoadCubemap(cubemapFaces, true); //load skyboxes in linear space

	//create materials
	TextureMaterial* planeMat = new TextureMaterial(planeDiffuse, nullptr, planeNormal, BlendMode::Opaque);

	TextureMaterial* nyraHeadMat = new TextureMaterial(headDiffuse, headSpecular, headNormal, BlendMode::Cutout);
	TextureMaterial* nyraBodyMat = new TextureMaterial(bodyDiffuse, bodySpecular, bodyNormal, BlendMode::Opaque);

	TextureMaterial* fernMat = new TextureMaterial(fernDiffuse, BlendMode::Cutout);
	TextureMaterial* grassMat1 = new TextureMaterial(grass1Diffuse, BlendMode::Cutout);
	TextureMaterial* grassMat2 = new TextureMaterial(grass2Diffuse, BlendMode::Cutout);

	TextureMaterial* rockMat = new TextureMaterial(rockDiffuse, BlendMode::Opaque);
	TextureMaterial* benchMat = new TextureMaterial(benchDiffuse, benchSpecular, benchNormal, BlendMode::Opaque);
	TextureMaterial* stumpMat = new TextureMaterial(stumpDiffuse, nullptr, stumpNormal, BlendMode::Opaque);
	TextureMaterial* logMat = new TextureMaterial(logDiffuse,nullptr, logNormal, BlendMode::Opaque);

	TextureMaterial* brickMat = new TextureMaterial(brickDiffuse, nullptr, brickNormal, BlendMode::Opaque);
	brickMat->setHeightMap(brickHeight);
	brickMat->setHeightScale(0.1f);
	brickMat->setFlipNormals(true);

	//create components for each node and fill with data
	CameraComponent* cameraComponent = new CameraComponent(glm::perspective(glm::radians(45.0f), (float)Window::ScreenWidth / (float)Window::ScreenHeight, 0.1f, 100.0f), 45.0f, 0.1f, 100.0f, 5.0f, 25.0f);

	LightComponent* directionalLightComponent = new LightComponent(LightType::Directional);
	directionalLightComponent->lightAmbient = glm::vec3(0.1f);
	directionalLightComponent->lightDiffuse = glm::vec3(0.5f);
	directionalLightComponent->lightSpecular = glm::vec3(0.1f);
	directionalLightComponent->lightDirection = glm::vec3(1.0f, -2.0f, -1.0f);

	RenderComponent* nyraHeadRenderComponent = new RenderComponent(nyraHead, nyraHeadMat);
	RenderComponent* nyraBodyRenderComponent = new RenderComponent(nyraBody, nyraBodyMat);

	RenderComponent* fern1RenderComponent = new RenderComponent(fernModel, fernMat);
	RenderComponent* fern2RenderComponent = new RenderComponent(fernModel, fernMat);
	RenderComponent* fern3RenderComponent = new RenderComponent(fernModel, fernMat);

	RenderComponent* grass1RenderComponent = new RenderComponent(grassModel1, grassMat1);
	RenderComponent* grass2RenderComponent = new RenderComponent(grassModel1, grassMat1);
	RenderComponent* grass3RenderComponent = new RenderComponent(grassModel1, grassMat1);
	RenderComponent* grass4RenderComponent = new RenderComponent(grassModel2, grassMat2);
	RenderComponent* grass5RenderComponent = new RenderComponent(grassModel2, grassMat2);
	RenderComponent* grass6RenderComponent = new RenderComponent(grassModel2, grassMat2);

	RenderComponent* rockRenderComponent = new RenderComponent(rockModel, rockMat);
	RenderComponent* benchRenderComponent = new RenderComponent(benchModel, benchMat);
	RenderComponent* stumpRenderComponent = new RenderComponent(stumpModel, stumpMat);
	RenderComponent* logRenderComponent = new RenderComponent(logModel, logMat);

	RenderComponent* brickRenderComponent = new RenderComponent(planeModel, brickMat);

	//add components to their respective nodes
	mainCamera->addComponent(cameraComponent);
	directionalLight->addComponent(directionalLightComponent);

	head->addComponent(nyraHeadRenderComponent);
	body->addComponent(nyraBodyRenderComponent);

	fern1->addComponent(fern1RenderComponent);
	fern2->addComponent(fern2RenderComponent);
	fern3->addComponent(fern3RenderComponent);

	grass1->addComponent(grass1RenderComponent);
	grass2->addComponent(grass2RenderComponent);
	grass3->addComponent(grass3RenderComponent);
	grass4->addComponent(grass4RenderComponent);
	grass5->addComponent(grass5RenderComponent);
	grass6->addComponent(grass6RenderComponent);

	rock->addComponent(rockRenderComponent);
	bench->addComponent(benchRenderComponent);
	stump->addComponent(stumpRenderComponent);
	log->addComponent(logRenderComponent);

	bricks->addComponent(brickRenderComponent);

	//add nodes to the world
	world->addChild(mainCamera);
	world->addChild(directionalLight);

	nyra->addChild(head);
	nyra->addChild(body);
	world->addChild(nyra);

	world->addChild(fern1);
	world->addChild(fern2);
	world->addChild(fern3);

	world->addChild(grass1);
	world->addChild(grass2);
	world->addChild(grass3);
	world->addChild(grass4);
	world->addChild(grass5);
	world->addChild(grass6);

	world->addChild(rock);
	world->addChild(bench);
	world->addChild(stump);
	world->addChild(log);

	world->addChild(bricks);

	//add the ground
	const unsigned int rows = 7;
	const unsigned int cols = 7;

	Node* plane;
	RenderComponent* planeRenderComponent;

	for(unsigned int i = 0; i < rows; i++) {
		for(unsigned int j = 0; j < cols; j++) {
			plane = new Node(glm::vec3(2.0f * i - cols / 2.0f - 2.0f, 0.0f, 2.0f * j - rows / 2.0f - 2.0f), "plane " + std::to_string(cols * i + j));
			planeRenderComponent = new RenderComponent(planeModel, planeMat);

			plane->addComponent(planeRenderComponent);

			ground->addChild(plane);
		}
	}

	world->addChild(ground);

	//set main camera, (main) directional light and skybox
	manager->setMainCamera(mainCamera);
	manager->setDirectionalLight(directionalLight);
	manager->setSkybox(skybox, false);

	std::cout << "Scene initialized" << std::endl;
}
