#ifndef NODE_H
#define NODE_H

#include <glm/glm.hpp>

#include <string>
#include <vector>

class Node {
	public:
		Node(std::string name, int id);
		~Node();

		std::string getName();
		int getID();
		Node* getParent();
		std::vector<Node*>* getChildren();
		Node* getChildAt(unsigned int index);
		
		virtual void addChild(Node* node);
		virtual void update(glm::mat4 parentTransform);

	protected:
		std::string _name;
		int _id;
		Node* _parent;
		std::vector<Node*> _children;

		virtual void _setParent(Node* node);

};

#endif