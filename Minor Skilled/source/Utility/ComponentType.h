#ifndef COMPONENTTYPE_H
#define COMPONENTTYPE_H

enum ComponentType {
	Transform = 1 << 0, //00000001
	Light = 1 << 1, //00000010
	Render = 1 << 2, //00000100
	Camera = 1 << 3 //00001000
};

#endif