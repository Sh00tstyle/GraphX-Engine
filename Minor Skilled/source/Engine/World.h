#ifndef WORLD_H
#define WORLD_H

#include "../Engine/Node.h"

class World: public Node {
	public:
		World();
		~World();

		virtual void setParent(Node* node);
		virtual void addChild(Node* node);
		virtual void update();

};

#endif