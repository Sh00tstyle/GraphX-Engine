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

		void setCastsShadows(bool value);
		bool getCastsShadows();

		virtual void drawForward(glm::mat4& modelMatrix) = 0;
		virtual void drawDeferred(glm::mat4& modelMatrix) = 0;

	protected:
		Material(BlendMode blendMode, bool castsShadows);

		BlendMode _blendMode;
		bool _castsShadows;

		virtual void _initShader() = 0;

};

#endif