#ifndef COMPONENTTYPE_H
#define COMPONENTTYPE_H

enum ComponentType {
	Light = 1 << 0,
	Render = 1 << 1,
	Camera = 1 << 2
};

#endif