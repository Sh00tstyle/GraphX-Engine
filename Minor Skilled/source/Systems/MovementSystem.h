#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../Engine/System.h"

class MovementSystem: public System {
	public:
		MovementSystem();
		~MovementSystem();

		virtual void update();
};

#endif