#ifndef COLORMATERIAL_H
#define COLORMATERIAL_H

#include "../Engine/Material.h"

#include <glm/glm.hpp>

class ColorMaterial : public Material {
	public:
		ColorMaterial(glm::vec3 color);
		~ColorMaterial();

		glm::vec3 getColor();
		void setColor(glm::vec3 color);

		virtual void draw();

	private:
		glm::vec3 _color;

};

#endif