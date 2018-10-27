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

		glm::vec3 getLocalPosition();
		glm::vec3 getWorldPosition();

		//matrix decomposition: https://stackoverflow.com/questions/17918033/glm-decompose-mat4-into-translation-and-rotation

};

#endif