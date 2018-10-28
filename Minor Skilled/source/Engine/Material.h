#ifndef MATERIAL_H
#define MATERIAL_H

#include <map>

#include <glm/glm.hpp>

class Shader;
class LightComponent;

class Material {
	public:
		~Material();

		virtual void draw(glm::mat4& modelMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix, glm::vec3& cameraPos, std::map<LightComponent*, glm::vec3>& lights) = 0;

	protected:
		Material();
};

#endif