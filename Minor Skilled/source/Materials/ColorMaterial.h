#ifndef COLORMATERIAL_H
#define COLORMATERIAL_H

#include "../Engine/Material.h"

#include <glm/glm.hpp>

class Shader;

class ColorMaterial : public Material {
	public:
		ColorMaterial(glm::vec3 color);
		~ColorMaterial();

		glm::vec3 getColor();
		void setColor(glm::vec3 color);

		virtual void draw(glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

	private:
		static Shader* _shader;

		glm::vec3 _color;

};

#endif