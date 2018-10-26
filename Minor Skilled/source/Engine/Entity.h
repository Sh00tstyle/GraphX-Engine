#ifndef ENTITY_H
#define ENTITY_H

#include <bitset>
#include <string>
#include <map>
#include <vector>

#include "../Utility/ComponentType.h"

class Component;

class Entity {
	public:
		Entity(std::string name, int id);
		~Entity();

		std::string getName();
		int getID();
		std::bitset<8> getComponentMask();
		Component* getComponent(ComponentType type);
		std::vector<Component*> getComponents(std::bitset<8> mask);
		void addComponent(Component* component);
		void removeComponent(ComponentType type);
		void removeComponents(std::bitset<8> mask);

	private:
		std::string _name;
		int _id;
		std::bitset<8> _componentMask;
		std::map<ComponentType, Component*> _components;

};

#endif