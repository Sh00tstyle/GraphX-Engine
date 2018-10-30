#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

class Transform {
	public:
		Transform(glm::vec3 localPosition);
		~Transform();

		glm::mat4 localTransform;
		glm::mat4 worldTransform;

		void decompose();
		void translate(glm::vec3 translation);
		void rotate(float angle, glm::vec3 axis);
		void scale(glm::vec3 scale);

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