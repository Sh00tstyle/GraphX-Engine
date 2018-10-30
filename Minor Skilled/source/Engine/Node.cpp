#include "Node.h"

#include "../Engine/Component.h"
#include "../Engine/Transform.h"

#include "../Utility/ComponentType.h"

Node::Node(glm::vec3 localPosition, std::string name):_transform(new Transform(localPosition)), _name(name), _parent(nullptr) {
}

Node::~Node() {
	for(unsigned int i = 0; _children.size(); i++) {
		delete _children[i];
	}
}

std::string Node::getName() {
	return _name;
}

Node* Node::getParent() {
	return _parent;
}

Transform* Node::getTransform() {
	return _transform;
}

std::vector<Node*>* Node::getChildren() {
	return &_children;
}

Node* Node::getChildAt(unsigned int index) {
	return _children[index];
}

Component* Node::getComponent(ComponentType type) {
	return _components[type];
}

bool Node::hasComponent(ComponentType type) {
	std::bitset<8> typeMask = type;

	return ((_componentMask & typeMask) == typeMask);
}

void Node::addChild(Node* node) {
	node->_setParent(this);

	_children.push_back(node);
}

void Node::removeChild(Node* node) {
	for(unsigned int i = 0; i < _children.size(); i++) {
		if(_children[i] == node) {
			_children.erase(_children.begin() + i);
			return;
		}
	}
}

void Node::addComponent(Component * component) {
	_components[component->getComponentType()] = component;
	component->setOwner(this);

	_componentMask |= component->getComponentType();
}

void Node::update(glm::mat4& parentTransform, std::vector<Node*>& renderables, std::vector<Node*>& lights, std::vector<Node*>& cameras) {
	_transform->decompose();

	//update components
	Component* component;

	for(std::map<ComponentType, Component*>::iterator it = _components.begin(); it != _components.end(); it++) {
		component = it->second;

		if(component != nullptr) component->update(renderables, lights, cameras);
	}

	//calculate model matrix
	_transform->worldTransform = parentTransform * _transform->localTransform;

	//update children
	for(unsigned int i = 0; i < _children.size(); i++) {
		_children[i]->update(_transform->worldTransform, renderables, lights, cameras);
	}
}

void Node::_setParent(Node * node) {
	_parent = node;
}