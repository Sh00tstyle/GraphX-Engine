#include "PBRMaterial.h"

#include "../Engine/Shader.h"
#include "../Engine/Texture.h"

#include "../Utility/Filepath.h"
#include "../Utility/RenderSettings.h"

Shader* PBRMaterial::_ForwardShader = nullptr;
Shader* PBRMaterial::_DeferredShader = nullptr;

PBRMaterial::PBRMaterial(Texture * albedoMap, Texture * normalMap, Texture * metallicMap, Texture * roughnessMap, Texture * aoMap, BlendMode blendMode) : Material(MaterialType::PBR, blendMode, true),
_albedoMap(albedoMap), _normalMap(normalMap), _metallicMap(metallicMap), _roughnessMap(roughnessMap), _aoMap(aoMap), _emissionMap(nullptr), _heightMap(nullptr), _F0(glm::vec3(0.04f)), _refractionFactor(0.0f), _heightScale(0.0f) {
	_initShader();
}

PBRMaterial::~PBRMaterial() {
	delete _albedoMap;
	delete _normalMap;
	delete _metallicMap;
	delete _roughnessMap;
	delete _aoMap;
}

Texture * PBRMaterial::getAlbedoMap() {
	return _albedoMap;
}

Texture * PBRMaterial::getNormalMap() {
	return _normalMap;
}

Texture * PBRMaterial::getMetallicMap() {
	return _metallicMap;
}

Texture * PBRMaterial::getRoughnessMap() {
	return _roughnessMap;
}

Texture * PBRMaterial::getAoMap() {
	return _aoMap;
}

Texture * PBRMaterial::getEmissionMap() {
	return _emissionMap;
}

Texture * PBRMaterial::getHeightMap() {
	return _heightMap;
}

glm::vec3 PBRMaterial::getF0() {
	return _F0;
}

float PBRMaterial::getRefractionFactor() {
	return _refractionFactor;
}

float PBRMaterial::getHeightScale() {
	return _heightScale;
}

void PBRMaterial::setAlbedoMap(Texture * albedoMap) {
	_albedoMap = albedoMap;
}

void PBRMaterial::setNormalMap(Texture * normalMap) {
	_normalMap = normalMap;
}

void PBRMaterial::setMetallicMap(Texture * metallicMap) {
	_metallicMap = metallicMap;
}

void PBRMaterial::setRoughnessMap(Texture * roughnessMap) {
	_roughnessMap = roughnessMap;
}

void PBRMaterial::setAoMap(Texture * aoMap) {
	_aoMap = aoMap;
}

void PBRMaterial::setEmissionMap(Texture * emissionMap) {
	_emissionMap = emissionMap;
}

void PBRMaterial::setHeightMap(Texture * heightMap) {
	_heightMap = heightMap;
}

void PBRMaterial::setF0(glm::vec3 F0) {
	_F0 = F0;
}

void PBRMaterial::setRefractionFactor(float refractionFactor) {
	_refractionFactor = refractionFactor;
}

void PBRMaterial::setHeightScale(float heightScale) {
	_heightScale = heightScale;
}

void PBRMaterial::drawSimple(Shader * shader) {
	//set environment shader properties
	shader->setBool("useTexture", true);

	glActiveTexture(GL_TEXTURE0);
	_albedoMap->bind(GL_TEXTURE_2D);
}

void PBRMaterial::drawForward(glm::mat4& modelMatrix) {
	_ForwardShader->use();

	//set model matrix
	_ForwardShader->setMat4("modelMatrix", modelMatrix);

	//set material textures and bools
	glActiveTexture(GL_TEXTURE0); //albedo

	if(_albedoMap != nullptr) {
		_albedoMap->bind(GL_TEXTURE_2D);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		std::cout << "ERROR: Albedo map in PBR shader is missing" << std::endl;
	}

	glActiveTexture(GL_TEXTURE1); //normal

	if(_normalMap != nullptr) {
		_normalMap->bind(GL_TEXTURE_2D);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		std::cout << "ERROR: Normal map in PBR shader is missing" << std::endl;
	}

	glActiveTexture(GL_TEXTURE2); //metallic

	if(_metallicMap != nullptr) {
		_metallicMap->bind(GL_TEXTURE_2D);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		std::cout << "ERROR: Metallic map in PBR shader is missing" << std::endl;
	}

	glActiveTexture(GL_TEXTURE3); //roughness

	if(_roughnessMap != nullptr) {
		_roughnessMap->bind(GL_TEXTURE_2D);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		std::cout << "ERROR: Roughness map in PBR shader is missing" << std::endl;
	}

	glActiveTexture(GL_TEXTURE4); //ao

	if(_aoMap != nullptr) {
		_aoMap->bind(GL_TEXTURE_2D);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		std::cout << "ERROR: AO map in PBR shader is missing" << std::endl;
	}

	glActiveTexture(GL_TEXTURE5); //emission

	if(_emissionMap != nullptr) {
		_emissionMap->bind(GL_TEXTURE_2D);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glActiveTexture(GL_TEXTURE6); //height

	if(_heightMap != nullptr) {
		_heightMap->bind(GL_TEXTURE_2D);
		_ForwardShader->setBool("material.hasHeight", true);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		_ForwardShader->setBool("material.hasHeight", false);
	}

	//set material properties
	_ForwardShader->setFloat("material.refractionFactor", _refractionFactor);
	_ForwardShader->setFloat("material.heightScale", _heightScale);
	_ForwardShader->setInt("material.blendMode", _blendMode);

	_ForwardShader->setFloat("maxReflectionLod", (float)(RenderSettings::MaxMipLevels - 1));
}

void PBRMaterial::drawDeferred(glm::mat4 & modelMatrix) {
	_DeferredShader->use();

	//set model matrix
	_DeferredShader->setMat4("modelMatrix", modelMatrix);

	//set material textures and bools
	glActiveTexture(GL_TEXTURE0); //albedo

	if(_albedoMap != nullptr) {
		_albedoMap->bind(GL_TEXTURE_2D);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		std::cout << "ERROR: Albedo map in PBR shader is missing" << std::endl;
	}

	glActiveTexture(GL_TEXTURE1); //normal

	if(_normalMap != nullptr) {
		_normalMap->bind(GL_TEXTURE_2D);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		std::cout << "ERROR: Normal map in PBR shader is missing" << std::endl;
	}

	glActiveTexture(GL_TEXTURE2); //metallic

	if(_metallicMap != nullptr) {
		_metallicMap->bind(GL_TEXTURE_2D);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		std::cout << "ERROR: Metallic map in PBR shader is missing" << std::endl;
	}

	glActiveTexture(GL_TEXTURE3); //roughness

	if(_roughnessMap != nullptr) {
		_roughnessMap->bind(GL_TEXTURE_2D);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		std::cout << "ERROR: Roughness map in PBR shader is missing" << std::endl;
	}

	glActiveTexture(GL_TEXTURE4); //ao

	if(_aoMap != nullptr) {
		_aoMap->bind(GL_TEXTURE_2D);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		std::cout << "ERROR: AO map in PBR shader is missing" << std::endl;
	}

	glActiveTexture(GL_TEXTURE5); //emission

	if(_emissionMap != nullptr) {
		_emissionMap->bind(GL_TEXTURE_2D);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glActiveTexture(GL_TEXTURE6); //height

	if(_heightMap != nullptr) {
		_heightMap->bind(GL_TEXTURE_2D);
		_DeferredShader->setBool("material.hasHeight", true);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		_DeferredShader->setBool("material.hasHeight", false);
	}

	//set material properties
	_DeferredShader->setFloat("material.refractionFactor", _refractionFactor);
	_DeferredShader->setFloat("material.heightScale", _heightScale);

	_DeferredShader->setFloat("maxReflectionLod", (float)(RenderSettings::MaxMipLevels - 1));
}

void PBRMaterial::_initShader() {
	if(_ForwardShader == nullptr) {
		_ForwardShader = new Shader(Filepath::ShaderPath + "material shader/forward/pbr.vs", Filepath::ShaderPath + "material shader/forward/pbr.fs");

		_ForwardShader->use();
		_ForwardShader->setInt("material.albedo", 0);
		_ForwardShader->setInt("material.normal", 1);
		_ForwardShader->setInt("material.metallic", 2);
		_ForwardShader->setInt("material.roughness", 3);
		_ForwardShader->setInt("material.ao", 4);
		_ForwardShader->setInt("material.emission", 5);
		_ForwardShader->setInt("material.height", 6);

		_ForwardShader->setInt("irradianceMap", 7);
		_ForwardShader->setInt("prefilterMap", 8);
		_ForwardShader->setInt("brdfLUT", 9);

		_ForwardShader->setInt("shadowMap", 10); //assign to slot 10, so that it shares it with the other materials which have more textures

		for(unsigned int i = 0; i < RenderSettings::MaxCubeShadows; i++) {
			_ForwardShader->setInt("shadowCubemaps[" + std::to_string(i) + "]", 11 + i);
		}

		_ForwardShader->setUniformBlockBinding("matricesBlock", 0); //set uniform block "matrices" to binding point 0
		_ForwardShader->setUniformBlockBinding("dataBlock", 1); //set uniform block "data" to binding point 1

		_ForwardShader->setShaderStorageBlockBinding("lightsBlock", 2); //set shader storage block "lights" to binding point 2
	}

	if(_DeferredShader == nullptr) {
		_DeferredShader = new Shader(Filepath::ShaderPath + "material shader/deferred/pbr.vs", Filepath::ShaderPath + "material shader/deferred/pbr.fs");

		_DeferredShader->use();
		_DeferredShader->setInt("material.albedo", 0);
		_DeferredShader->setInt("material.normal", 1);
		_DeferredShader->setInt("material.metallic", 2);
		_DeferredShader->setInt("material.roughness", 3);
		_DeferredShader->setInt("material.ao", 4);
		_DeferredShader->setInt("material.emission", 5);
		_DeferredShader->setInt("material.height", 6);

		_DeferredShader->setInt("irradianceMap", 7);
		_DeferredShader->setInt("prefilterMap", 8);

		_DeferredShader->setUniformBlockBinding("matricesBlock", 0); //set uniform block "matrices" to binding point 0
		_DeferredShader->setUniformBlockBinding("dataBlock", 1); //set uniform block "data" to binding point 1
	}
}
