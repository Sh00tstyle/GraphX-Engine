#include "Node.h"

#include <iostream>
#include <string>

#include "../Engine/Entity.h"
#include "../Engine/EntityManager.h"

#include "../Components/TransformComponent.h"

#include "../Utility/ComponentType.h"

Node::Node(std::string name, int id):_name(name), _id(id), _parent(nullptr) {
}

Node::~Node() {
	for(unsigned int i = 0; _children.size(); i++) {
		delete _children[i];
	}
}

std::string Node::getName() {
	return _name;
}

int Node::getID() {
	return _id;
}

Node * Node::getParent() {
	return _parent;
}

std::vector<Node*>* Node::getChildren() {
	return &_children;
}

Node * Node::getChildAt(unsigned int index) {
	return _children[index];
}

void Node::addChild(Node* node) {
	node->_setParent(this);

	_children.push_back(node);
}

void Node::update(glm::mat4 parentTransform) {
	//get relevant data
	Entity* nodeEntity = EntityManager::GetEntityByID(_id); //can be cached, maybe use lazy init?
	TransformComponent* transformComponent = (TransformComponent*)nodeEntity->getComponent(ComponentType::Transform); //can be cached, maybe use lazy init?

	//calculate the world transform for this node
	glm::mat4 worldTransform = parentTransform * transformComponent->localTransform;

	//update the world transform in the component
	transformComponent->worldTransform = worldTransform;

	//print world position for debug
	//glm::vec3 worldPos = transformComponent->getWorldPosition();
	//std::cout << _name + " world pos: " + std::to_string(worldPos.x) + ", " + std::to_string(worldPos.y) + ", " + std::to_string(worldPos.z) << std::endl;

	for(unsigned int i = 0; i < _children.size(); i++) {
		_children[i]->update(worldTransform); //pass world transform down to children
	}
}

void Node::_setParent(Node * node) {
	_parent = node;
}