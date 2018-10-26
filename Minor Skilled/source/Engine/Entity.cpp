#include "Entity.h"

#include <iterator>

#include "../Engine/Component.h"

Entity::Entity(std::string name, int id):_name(name), _id(id) {
	_componentMask = 0; //init to 00000000 (no component type)
}

Entity::~Entity() {
	for(std::map<ComponentType, Component*>::iterator it = _components.begin(); it != _components.end(); it++) {
		delete it->second;
	}
}

std::string Entity::getName() {
	return _name;
}

int Entity::getID() {
	return _id;
}

std::bitset<8> Entity::getComponentMask() {
	return _componentMask;
}

Component * Entity::getComponent(ComponentType type) {
	return _components[type];
}

std::vector<Component*> Entity::getComponents(std::bitset<8> mask) {
	std::vector<Component*> components;
	std::bitset<8> currentMask;

	//looking all components that are requested in the mask
	//https://stackoverflow.com/questions/26281979/c-loop-through-map
	for(std::map<ComponentType, Component*>::iterator it = _components.begin(); it != _components.end(); it++) {
		currentMask = it->first;

		if((currentMask & mask) == currentMask) components.push_back(it->second);
	}

	return components; //return copy
}

void Entity::addComponent(Component* component) {
	ComponentType componentType = component->getComponentType();

	_components[componentType] = component; //update map
	_componentMask |= componentType; //add type to mask
}

void Entity::removeComponent(ComponentType type) {
	Component* component = _components[type];

	_components.erase(type); //update map
	_componentMask &= ~type; //remove type from mask

	delete component;
}

void Entity::removeComponents(std::bitset<8> mask) {
	std::bitset<8> currentMask;
	std::bitset<8> removedMasks = 0; //init to 00000000 (nothing to delete)
	Component* component;

	//looking for all components that are specified in the mask and deleting them from the map
	//https://stackoverflow.com/questions/8234779/how-to-remove-from-a-map-while-iterating-it
	for(auto it = _components.cbegin(); it != _components.cend();) {
		currentMask = it->first;

		if((currentMask & mask) == currentMask) {
			component = it->second;

			_components.erase(it++);
			removedMasks |= currentMask; //add current mask to update the component mask later

			delete component;
		} else {
			++it;
		}
	}

	_componentMask &= ~removedMasks; //remove all removed component types from the component mask
}