#include "EntityManager.h"

#include "../Engine/Entity.h"

std::map<int, Entity*> EntityManager::_Entities;

Entity * EntityManager::GetEntityByID(int id) {
	return _Entities[id];
}

Entity * EntityManager::GetEntityByName(std::string name) {
	Entity* entity;

	for(std::map<int, Entity*>::iterator it = _Entities.begin(); it != _Entities.end(); it++) {
		entity = it->second;
		if(entity->getName() == name) return entity;
	}

	return nullptr;
}

std::vector<Entity*> EntityManager::GetEntitiesByMask(std::bitset<8> componentMask) {
	std::vector<Entity*> entities;
	std::bitset<8> currentMask;

	for(std::map<int, Entity*>::iterator it = _Entities.begin(); it != _Entities.end(); it++) {
		currentMask = it->second->getComponentMask();

		if((currentMask & componentMask) == componentMask) entities.push_back(it->second); //add entities that have at least the requested components from the mask
	}

	return entities; //return copy
}

void EntityManager::AddEntity(Entity * entity) {
	_Entities[entity->getID()] = entity;
}

void EntityManager::RemoveEntity(int id) {
	Entity* entity = _Entities[id];

	_Entities.erase(id);

	delete entity;
}

void EntityManager::DeleteEntities() {
	Entity* entity;

	for(std::map<int, Entity*>::iterator it = _Entities.begin(); it != _Entities.end(); it++) {
		entity = it->second;

		delete entity;
	}
}
