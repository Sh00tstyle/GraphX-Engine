#ifndef WORLD_H
#define WORLD_H

#include "../Engine/Node.h"

class World: public Node {
	public:
		World();
		~World();

		void update();

};

#endif