#ifndef TEXTUREMATERIAL_H
#define TEXTUREMATERIAL_H

#include <glm/glm.hpp>

#include "../Engine/Material.h"

class Shader;
class Texture;

class TextureMaterial : public Material {
	public:
		TextureMaterial(Texture* diffuseMap, Texture* specularMap, Texture* normalMap, Texture* emissionMap);
		~TextureMaterial();

		Texture* getDiffuseMap();
		Texture* getSpecularMap();
		Texture* getNormalMap();
		Texture* getEmissionMap();
		void setDiffuseMap(Texture* diffuseMap);
		void setSpecularMap(Texture* specularMap);
		void setNormalMap(Texture* normalMap);
		void setEmissionMap(Texture* emissionMap);

		virtual void draw(glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

	private:
		static Shader* _shader;

		Texture* _diffuseMap;
		Texture* _specularMap;
		Texture* _normalMap;
		Texture* _emissionMap;

};

#endif