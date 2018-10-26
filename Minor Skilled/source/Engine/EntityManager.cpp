#include "EntityManager.h"

#include "../Engine/Entity.h"

std::map<int, Entity*> EntityManager::_Entities;

Entity * EntityManager::GetEntityByID(int id) {
	return _Entities[id];
}

std::vector<Entity*> EntityManager::GetEntitiesByMask(std::bitset<8> componentMask) {
	std::vector<Entity*> entities;
	std::bitset<8> currentMask;

	for(std::map<int, Entity*>::iterator it = _Entities.begin(); it != _Entities.end(); it++) {
		currentMask = it->second->getComponentMask();

		if((currentMask & componentMask) == currentMask) entities.push_back(it->second);
	}

	return entities; //return copy
}

void EntityManager::AddEntity(int id, Entity * entity) {
	_Entities[id] = entity;
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
