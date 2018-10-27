#include "DemoScene.h"

#include <iostream>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "../Engine/Window.h"
#include "../Engine/Node.h"
#include "../Engine/World.h"
#include "../Engine/Entity.h"
#include "../Engine/EntityManager.h"
#include "../Engine/Component.h"
#include "../Engine/Texture.h"

#include "../Components/TransformComponent.h"
#include "../Components/CameraComponent.h"
#include "../Components/RenderComponent.h"

#include "../Materials/ColorMaterial.h"
#include "../Materials/TextureMaterial.h"

#include "../Utility/Filepath.h"

DemoScene::DemoScene():Scene() {
}

DemoScene::~DemoScene() {
}

void DemoScene::_initializeScene() {
	std::cout << "Initializing Scene" << std::endl;

	//-----SCENE GRAPH-----
	std::cout << "Setting up Scene Graph" << std::endl;

	//create scene objects which represent graph nodes
	Node* camera = new Node("camera", 0);
	Node* testNode1 = new Node("test1", 1);
	Node* testNode2 = new Node("test2", 2);
	
	//set child/parent relationships
	testNode1->addChild(testNode2);

	//add nodes to the scene graph
	_world->addChild(camera);
	_world->addChild(testNode1);

	//-----ENTIY COMPONENT SYSTEM-----
	std::cout << "Setting up Entity Component System" << std::endl;

	//setting up the camera entity
	Entity* cameraEntity = new Entity(camera->getName(), camera->getID());

	//create entities for each scene object
	Entity* testEntity1 = new Entity(testNode1->getName(), testNode1->getID());
	Entity* testEntity2 = new Entity(testNode2->getName(), testNode2->getID());

	//create components for each entity
	TransformComponent* cameraTransformComponent = new TransformComponent();
	CameraComponent* cameraComponent = new CameraComponent();

	TransformComponent* transformComponent1 = new TransformComponent();
	RenderComponent* renderComponent1 = new RenderComponent();

	TransformComponent* transformComponent2 = new TransformComponent();

	//set data in each component
	glm::mat4 cameraTransform = glm::mat4(1.0f);
	cameraTransformComponent->localTransform = glm::translate(cameraTransform, glm::vec3(0.0f, 0.0f, 3.0f));

	cameraComponent->fieldOfView = 45.0f;
	cameraComponent->projectionMatrix = glm::perspective(glm::radians(cameraComponent->fieldOfView), (float)Window::ScreenWidth / (float)Window::ScreenHeight, 0.1f, 100.0f);

	glm::mat4 transform1 = glm::mat4(1.0f);
	transform1 = glm::scale(transform1, glm::vec3(0.1f));
	transformComponent1->localTransform = glm::translate(transform1, glm::vec3(1.0f, 0.0f, 0.0));

	renderComponent1->model = Model::LoadModel(Filepath::ModelPath + "cyborg/cyborg.obj");
	renderComponent1->material = new TextureMaterial(Texture::LoadTexture(Filepath::ModelPath + "cyborg/cyborg_diffuse.png"), nullptr, nullptr, nullptr);

	glm::mat4 transform2 = glm::mat4(1.0f);
	transformComponent2->localTransform = glm::translate(transform2, glm::vec3(-1.0f, 1.0f, 0.0)); //problem: hard to define local positions with a lot parents

	//add components to each entity
	cameraEntity->addComponent(cameraTransformComponent);
	cameraEntity->addComponent(cameraComponent);

	testEntity1->addComponent(transformComponent1);
	testEntity1->addComponent(renderComponent1);

	testEntity2->addComponent(transformComponent2);

	//register each entity in the entity manager
	EntityManager::AddEntity(cameraEntity);
	EntityManager::AddEntity(testEntity1);
	EntityManager::AddEntity(testEntity2);

	std::cout << "Scene initialized" << std::endl;
}
