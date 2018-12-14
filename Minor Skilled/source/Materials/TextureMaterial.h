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
		TextureMaterial(Texture* diffuseMap, BlendMode blendMode);
		TextureMaterial(Texture* diffuseMap, Texture* specularMap, Texture* normalMap, BlendMode blendMode);
		~TextureMaterial();

		Texture* getDiffuseMap();
		Texture* getSpecularMap();
		Texture* getNormalMap();
		Texture* getEmissionMap();
		Texture* getHeightMap();
		Texture* getReflectionMap();
		float& getShininess();
		float& getRefractionFactor();
		float& getHeightScale();

		void setDiffuseMap(Texture* diffuseMap);
		void setSpecularMap(Texture* specularMap);
		void setNormalMap(Texture* normalMap);
		void setEmissionMap(Texture* emissionMap);
		void setHeightMap(Texture* heightMap);
		void setReflectionMap(Texture* reflectionMap);
		void setShininess(float shininess);
		void setRefractionFactor(float refrationFactor);
		void setHeightScale(float heightScale);

		virtual void drawSimple(Shader* shader);
		virtual void drawForward(glm::mat4& modelMatrix);
		virtual void drawDeferred(glm::mat4& modelMatrix);

	private:
		static Shader* _ForwardShader;
		static Shader* _DeferredShader;

		Texture* _diffuseMap;
		Texture* _specularMap;
		Texture* _normalMap;
		Texture* _heightMap;
		Texture* _emissionMap;
		Texture* _reflectionMap;
		float _shininess;
		float _refractionFactor;
		float _heightScale;

		virtual void _initShader();

};

#endif