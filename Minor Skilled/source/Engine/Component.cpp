#include "Component.h"

Component::~Component() {
}

ComponentType Component::getComponentType() {
	return _componentType;
}

Component::Component(ComponentType type):_componentType(type) {
}
