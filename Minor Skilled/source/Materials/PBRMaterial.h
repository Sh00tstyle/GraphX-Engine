#ifndef PBRMATERIAL_H
#define PBRMATERIAL_H

#include "../Engine/Material.h"

class Shader;
class Texture;

class PBRMaterial : public Material {
	public:
		PBRMaterial(Texture* albedoMap, Texture* normalMap, Texture* metallicMap, Texture* roughnessMap, Texture* aoMap, BlendMode blendMode);
		~PBRMaterial();

		Texture* getAlbedoMap();
		Texture* getNormalMap();
		Texture* getMetallicMap();
		Texture* getRoughnessMap();
		Texture* getAoMap();
		Texture* getEmissionMap();
		Texture* getHeightMap();
		float getRefractionFactor();
		float getHeightScale();

		void setAlbedoMap(Texture* albedoMap);
		void setNormalMap(Texture* normalMap);
		void setMetallicMap(Texture* metallicMap);
		void setRoughnessMap(Texture* roughnessMap);
		void setAoMap(Texture* aoMap);
		void setEmissionMap(Texture* emissionMap);
		void setHeightMap(Texture* heightMap);
		void setRefractionFactor(float refractionFactor);
		void setHeightScale(float heightScale);

		virtual void drawSimple(Shader* shader);
		virtual void drawForward(glm::mat4& modelMatrix);
		virtual void drawDeferred(glm::mat4& modelMatrix);

	private:
		static Shader* _ForwardShader;
		static Shader* _DeferredShader;

		Texture* _albedoMap;
		Texture* _normalMap;
		Texture* _metallicMap;
		Texture* _roughnessMap;
		Texture* _aoMap;
		Texture* _emissionMap;
		Texture* _heightMap;
		float _refractionFactor;
		float _heightScale;

		virtual void _initShader();
};

#endif