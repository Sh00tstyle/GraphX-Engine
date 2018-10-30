#include "World.h"

#include "../Engine/Node.h"

World::World() {
}

World::~World() {
	for(unsigned int i = 0; i < _children.size(); i++) {
		delete _children[i];
	}
}

std::vector<Node*>* World::getChildren() {
	return &_children;
}

Node * World::getChildAt(unsigned int index) {
	return _children[index];
}

void World::addChild(Node* node) {
	_children.push_back(node);
}

void World::removeChild(Node* node) {
	for(unsigned int i = 0; i < _children.size(); i++) {
		if(_children[i] == node) {
			_children.erase(_children.begin() + i);
			return;
		}
	}
}

void World::update(std::vector<Node*>& renderables, std::vector<Node*>& lights, std::vector<Node*>& cameras) {
	glm::mat4 worldModel = glm::mat4(1.0f);

	for(unsigned int i = 0; i < _children.size(); i++) {
		_children[i]->update(worldModel,  renderables, lights, cameras);
	}
}
