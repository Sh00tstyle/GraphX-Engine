#ifndef WORLD_H
#define WORLD_H

#include <vector>

class Node;

class World {
	public:
		World();
		~World();

		std::vector<Node*>* getChildren();
		Node* getChildAt(unsigned int index);
		unsigned int getChildCount();
		void addChild(Node* node);
		void removeChild(Node* node);
		void update(std::vector<Node*>& renderables, std::vector<Node*>& lights);

		void erase();

	private:
		std::vector<Node*> _children;

};

#endif