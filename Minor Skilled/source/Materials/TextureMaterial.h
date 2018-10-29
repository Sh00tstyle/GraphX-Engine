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
		TextureMaterial();
		TextureMaterial(Texture* diffuseMap, Texture* specularMap, Texture* normalMap, Texture* emissionMap, float shininess);
		~TextureMaterial();

		static TextureMaterial* LoadMaterial(std::string path);

		Texture* getDiffuseMap();
		Texture* getSpecularMap();
		Texture* getNormalMap();
		Texture* getEmissionMap();
		float getShininess();

		void setDiffuseMap(Texture* diffuseMap);
		void setSpecularMap(Texture* specularMap);
		void setNormalMap(Texture* normalMap);
		void setEmissionMap(Texture* emissionMap);
		void setShininess(float shininess);

		virtual void draw(glm::mat4& modelMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix, glm::vec3& cameraPos, std::map<LightComponent*, glm::vec3>& lights);

	private:
		static Shader* _shader;

		Texture* _diffuseMap;
		Texture* _specularMap;
		Texture* _normalMap;
		Texture* _emissionMap;
		float _shininess;

};

#endif