#ifndef COMPONENT_H
#define COMPONENT_H

#include "../Utility/ComponentType.h"

class Component {
	public:
		~Component();

		ComponentType getComponentType();

	protected:
		Component(); //protected constructor to make it impossible to intantiate from this container class

		ComponentType _componentType;
};

#endif