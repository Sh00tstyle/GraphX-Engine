#ifndef COLORMATERIAL_H
#define COLORMATERIAL_H

#include "../Engine/Material.h"

#include <glm/glm.hpp>

class Shader;
class LightComponent;

class ColorMaterial : public Material {
	public:
		ColorMaterial(glm::vec3 diffuseColor);
		ColorMaterial(glm::vec3 ambientColor, glm::vec3 diffuseColor, float specular, float shininess = 32.0f, bool castsShadows = true);
		~ColorMaterial();

		glm::vec3& getAmbientColor();
		glm::vec3& getDiffuseColor();
		float& getSpecular();
		float& getShininess();

		void setAmbientColor(glm::vec3 ambientColor);
		void setDiffuseColor(glm::vec3 diffuseColor);
		void setSpecular(float specular);
		void setShininess(float shininess);

		virtual void drawSimple(Shader* shader);
		virtual void drawForward(glm::mat4& modelMatrix);
		virtual void drawDeferred(glm::mat4& modelMatrix);

	private:
		static Shader* _ForwardShader;
		static Shader* _DeferredShader;

		glm::vec3 _ambientColor;
		glm::vec3 _diffuseColor;
		float _specular;
		float _shininess;

		virtual void _initShader();

};

#endif