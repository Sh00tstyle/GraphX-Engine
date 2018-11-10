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

		virtual void draw(glm::mat4& modelMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix, glm::vec3& cameraPos, std::vector<std::pair<LightComponent*, glm::vec3>>& lights) = 0;

	protected:
		Material(BlendMode blendMode);

		BlendMode _blendMode;

};

#endif