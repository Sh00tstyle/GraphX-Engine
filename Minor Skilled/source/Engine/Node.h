#ifndef NODE_H
#define NODE_H

#include <bitset>
#include <string>
#include <vector>
#include <map>

#include <glm/glm.hpp>

#include "../Utility/ComponentType.h"

class Transform;
class Component;

class Node {
	public:
		Node(glm::vec3 localPosition, std::string name);
		~Node();

		std::string getName();
		Node* getParent();
		Transform* getTransform();
		std::vector<Node*>* getChildren();
		Node* getChildAt(unsigned int index);
		unsigned int getChildCount();
		Component* getComponent(ComponentType type);
		bool hasComponent(ComponentType type);
		
		virtual void addChild(Node* node);
		virtual void removeChild(Node* node);
		virtual void addComponent(Component* component);
		virtual void update(glm::mat4& parentTransform, std::vector<Node*>& renderables, std::vector<Node*>& lights);

	protected:
		std::string _name;
		Node* _parent;
		std::vector<Node*> _children;
		Transform* _transform;
		std::map<ComponentType, Component*> _components;

		virtual void _setParent(Node* node);

	private:
		std::bitset<8> _componentMask;


};

#endif