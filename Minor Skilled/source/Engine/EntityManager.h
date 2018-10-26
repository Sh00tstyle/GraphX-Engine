#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <map>
#include <vector>
#include <bitset>

class Entity;

class EntityManager {
	public:
		static Entity* GetEntityByID(int id);
		static std::vector<Entity*> GetEntitiesByMask(std::bitset<8> componentMask); //returns all Entity that have exactly the requested component mask
		static void AddEntity(int id, Entity* entity);
		static void RemoveEntity(int id);

		static void DeleteEntities();

	private:
		static std::map<int, Entity*> _Entities;

};

#endif