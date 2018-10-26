#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>

class Node {
	public:
		Node(std::string name, int id, Node* parent);
		~Node();

		std::string getName();
		int getID();
		Node* getParent();
		std::vector<Node*>* getChildren();
		Node* getChildAt(unsigned int index);

		virtual void setParent(Node* node);
		virtual void addChild(Node* node);
		virtual void update();

	protected:
		std::string _name;
		int _id;
		Node* _parent;
		std::vector<Node*> _children;

};

#endif