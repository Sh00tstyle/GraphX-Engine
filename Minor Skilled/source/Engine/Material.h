#ifndef MATERIAL_H
#define MATERIAL_H

#include <vector>

#include <glm/glm.hpp>

#include "../Utility/BlendMode.h"

class Shader;
class LightComponent;

class Material {
	public:
		~Material();

		BlendMode getBlendMode();
		void setBlendMode(BlendMode blendMode);

		virtual void draw(glm::mat4& modelMatrix) = 0;

	protected:
		Material(BlendMode blendMode);

		BlendMode _blendMode;

		virtual void _initShader() = 0;

};

#endif