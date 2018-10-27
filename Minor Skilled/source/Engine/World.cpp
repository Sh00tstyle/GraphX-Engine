#include "World.h"

#include "../Engine/Node.h"

World::World():Node("world", -1) {
}

World::~World() {
	for(unsigned int i = 0; i < _children.size(); i++) {
		delete _children[i];
	}
}

void World::update() {
	glm::mat4 localTransform = glm::mat4(1.0f); //the world localTransform will always be the identity matrix

	for(unsigned int i = 0; i < _children.size(); i++) {
		_children[i]->update(localTransform);
	}
}