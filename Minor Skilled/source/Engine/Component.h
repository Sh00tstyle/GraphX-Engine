#ifndef COMPONENT_H
#define COMPONENT_H

#include <vector>

#include "../Utility/ComponentType.h"

class Node;

class Component {
	public:
		~Component();

		ComponentType getComponentType();
		
		void setOwner(Node* owner);
		virtual void update(std::vector<Node*>& renderables, std::vector<Node*>& lights, std::vector<Node*>& cameras) = 0;

	protected:
		Component(ComponentType type);

		ComponentType _componentType;
		Node* _owner;
};

#endif