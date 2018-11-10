#include "Component.h"

#include "../Engine/Node.h"

Component::~Component() {
}

ComponentType Component::getComponentType() {
	return _componentType;
}

void Component::setOwner(Node * owner) {
	_owner = owner;
}

Node* Component::getOwner() {
	return _owner;
}

Component::Component(ComponentType type): _componentType(type) {
}
