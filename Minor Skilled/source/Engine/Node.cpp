#include "Node.h"

Node::Node(std::string name, int id, Node* parent):_name(name), _id(id), _parent(parent) {
}

Node::~Node() {
	for(unsigned int i = 0; _children.size(); i++) {
		delete _children[i];
	}
}

std::string Node::getName() {
	return _name;
}

int Node::getID() {
	return _id;
}

Node * Node::getParent() {
	return _parent;
}

std::vector<Node*>* Node::getChildren() {
	return &_children;
}

Node * Node::getChildAt(unsigned int index) {
	return _children[index];
}

void Node::setParent(Node * node) {
	_parent = node;
}

void Node::addChild(Node * node) {
	_children.push_back(node);
}

void Node::update() {
	//update the scene graph WITHOUT rendering
}
