#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include <glm/glm.hpp>

#include "../Engine/Component.h"

class TransformComponent : public Component {
	public:
		TransformComponent();
		~TransformComponent();

		glm::mat4 localTransform;
		glm::mat4 worldTransform;

};

#endif