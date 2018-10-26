#include "World.h"

#include "../Engine/Node.h"

World::World():Node("world", -1, nullptr) {
}

World::~World() {
	for(unsigned int i = 0; i < _children.size(); i++) {
		delete _children[i];
	}
}

void World::setParent(Node * node) {
	//left empty, we do not want to change the parent of the world node
}

void World::addChild(Node * node) {
	_children.push_back(node);
}

void World::update() {
}
