#ifndef COMPONENT_H
#define COMPONENT_H

#include <vector>

#include "../Utility/ComponentType.h"

class Node;

class Component {
	public:
		~Component();

		ComponentType getComponentType();
		
		virtual void setOwner(Node* owner);
		Node* getOwner();

		virtual void update() = 0;

	protected:
		Component(ComponentType type);

		ComponentType _componentType;
		Node* _owner;
};

#endif