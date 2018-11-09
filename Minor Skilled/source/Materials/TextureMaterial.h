#ifndef TEXTUREMATERIAL_H
#define TEXTUREMATERIAL_H

#include <string>

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Engine/Material.h"

class Shader;
class Texture;

class TextureMaterial : public Material {
	public:
		TextureMaterial(Texture* diffuseMap, float shininess = 32.0f);
		TextureMaterial(Texture* diffuseMap, Texture* specularMap = nullptr, Texture* normalMap = nullptr, Texture* heightMap = nullptr, Texture* emissionMap = nullptr, float shininess = 32.0f, float heightScale = 1.0f);
		~TextureMaterial();

		Texture* getDiffuseMap();
		Texture* getSpecularMap();
		Texture* getNormalMap();
		Texture* getEmissionMap();
		Texture* getHeightMap();
		float getShininess();
		float getHeightScale();

		void setDiffuseMap(Texture* diffuseMap);
		void setSpecularMap(Texture* specularMap);
		void setNormalMap(Texture* normalMap);
		void setEmissionMap(Texture* emissionMap);
		void setHeightMap(Texture* heightMap);
		void setShininess(float shininess);
		void setHeightScale(float heightScale);

		virtual void draw(glm::mat4& modelMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix, glm::vec3& cameraPos, std::map<LightComponent*, glm::vec3>& lights);

	private:
		static Shader* _shader;

		Texture* _diffuseMap;
		Texture* _specularMap;
		Texture* _normalMap;
		Texture* _heightMap;
		Texture* _emissionMap;
		float _shininess;
		float _heightScale;

		void _initShader();

};

#endif