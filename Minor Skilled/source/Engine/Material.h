#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

class Shader;

class Material {
	public:
		~Material();

		virtual void draw(glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projectionMatrix) = 0;

	protected:
		Material();
};

#endif