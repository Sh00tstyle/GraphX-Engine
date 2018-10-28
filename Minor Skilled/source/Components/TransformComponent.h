#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "../Engine/Component.h"

class TransformComponent : public Component {
	public:
		TransformComponent();
		~TransformComponent();

		glm::mat4 localTransform; //use this for transform behaviour in systems
		glm::mat4 worldTransform; //modifying this in a system will not do anything, since it is calculated based on the local transform in the scene graph

		void decompose();

		glm::vec3 getLocalScale();
		glm::quat getLocalRotation();
		glm::vec3 getLocalPosition(bool decomposed = false);
		glm::vec3 getLocalSkew();
		glm::vec3 getLocalPerspective();

		glm::vec3 getWorldScale();
		glm::quat getWorldRotation();
		glm::vec3 getWorldPosition(bool decomposed = false);
		glm::vec3 getWorldSkew();
		glm::vec3 getWorldPerspective();

	private:
		glm::vec3 _localScale;
		glm::quat _localRotation;
		glm::vec3 _localPosition;
		glm::vec3 _localSkew;
		glm::vec4 _localPerspective;

		glm::vec3 _worldScale;
		glm::quat _worldRotation;
		glm::vec3 _worldPosition;
		glm::vec3 _worldSkew;
		glm::vec4 _worldPerspective;

};

#endif