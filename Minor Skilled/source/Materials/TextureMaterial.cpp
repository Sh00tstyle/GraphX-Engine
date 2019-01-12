#include "TextureMaterial.h"

#include <glm/glm.hpp>

#include "../Engine/Texture.h"
#include "../Engine/Shader.h"

#include "../Components/LightComponent.h"

#include "../Utility/Filepath.h"
#include "../Utility/RenderSettings.h"

Shader* TextureMaterial::_ForwardShader = nullptr;
Shader* TextureMaterial::_DeferredShader = nullptr;

TextureMaterial::TextureMaterial(Texture* diffuseMap, BlendMode blendMode) :Material(MaterialType::Textures, blendMode, true), _diffuseMap(diffuseMap), _specularMap(nullptr),
_normalMap(nullptr), _emissionMap(nullptr), _heightMap(nullptr), _shininess(32.0f), _refractionFactor(0.0f), _heightScale(0.0f) {
	_initShader();
}

TextureMaterial::TextureMaterial(Texture * diffuseMap, Texture * specularMap, Texture * normalMap, BlendMode blendMode) :
	Material(MaterialType::Textures, blendMode, true),_diffuseMap(diffuseMap), _specularMap(specularMap), _normalMap(normalMap), _emissionMap(nullptr), _reflectionMap(nullptr), _heightMap(nullptr), _shininess(32.0f), _refractionFactor(0.0f), _heightScale(0.0f) {
	_initShader();
}

TextureMaterial::~TextureMaterial() {
	delete _diffuseMap;
	delete _specularMap;
	delete _normalMap;
	delete _emissionMap;
	delete _reflectionMap;
	delete _heightMap;
}

Texture * TextureMaterial::getDiffuseMap() {
	return _diffuseMap;
}

Texture * TextureMaterial::getSpecularMap() {
	return _specularMap;
}

Texture * TextureMaterial::getNormalMap() {
	return _normalMap;
}

Texture * TextureMaterial::getEmissionMap() {
	return _emissionMap;
}

Texture * TextureMaterial::getHeightMap() {
	return _heightMap;
}

Texture * TextureMaterial::getReflectionMap() {
	return _reflectionMap;
}

float& TextureMaterial::getShininess() {
	return _shininess;
}

float& TextureMaterial::getRefractionFactor() {
	return _refractionFactor;
}

float& TextureMaterial::getHeightScale() {
	return _heightScale;
}

void TextureMaterial::setDiffuseMap(Texture* diffuseMap) {
	_diffuseMap = diffuseMap;
}

void TextureMaterial::setSpecularMap(Texture* specularMap) {
	_specularMap = specularMap;
}

void TextureMaterial::setNormalMap(Texture* normalMap) {
	_normalMap = normalMap;
}

void TextureMaterial::setEmissionMap(Texture * emissionMap) {
	_emissionMap = emissionMap;
}

void TextureMaterial::setHeightMap(Texture * heightMap) {
	_heightMap = heightMap;
}

void TextureMaterial::setReflectionMap(Texture * reflectionMap) {
	_reflectionMap = reflectionMap;
}

void TextureMaterial::setShininess(float shininess) {
	_shininess = shininess;
}

void TextureMaterial::setRefractionFactor(float refractionFactor) {
	_refractionFactor = refractionFactor;
}

void TextureMaterial::setHeightScale(float heightScale) {
	_heightScale = heightScale;
}

void TextureMaterial::drawSimple(Shader* shader) {
	//set environment shader properties
	shader->setBool("useTexture", true);

	Texture::SetActiveUnit(0);
	_diffuseMap->bind();
}

void TextureMaterial::drawForward(glm::mat4& modelMatrix) {
	_ForwardShader->use();

	//set model matrix
	_ForwardShader->setMat4("modelMatrix", modelMatrix);

	//set material textures and bools
	Texture::SetActiveUnit(0); //diffuse

	if(_diffuseMap != nullptr) {
		_diffuseMap->bind();
	} else {
		Texture::Unbind(GL_TEXTURE_2D);
		std::cout << "ERROR: No diffuse map in the texture material. Ensure that there is at least a diffuse map present!" << std::endl;
	}
	
	Texture::SetActiveUnit(1); //specular

	if(_specularMap != nullptr) {
		_specularMap->bind();
		_ForwardShader->setBool("material.hasSpecular", true);
	} else {
		Texture::Unbind(GL_TEXTURE_2D);
		_ForwardShader->setBool("material.hasSpecular", false);
	}

	Texture::SetActiveUnit(2); //normal

	if(_normalMap != nullptr) {
		_normalMap->bind();
		_ForwardShader->setBool("material.hasNormal", true);
	} else {
		Texture::Unbind(GL_TEXTURE_2D);
		_ForwardShader->setBool("material.hasNormal", false);
	}

	Texture::SetActiveUnit(3); //emission

	if(_emissionMap != nullptr) {
		_emissionMap->bind();
	} else {
		Texture::Unbind(GL_TEXTURE_2D);
	}

	Texture::SetActiveUnit(4); //reflection

	if(_reflectionMap != nullptr) {
		_reflectionMap->bind();
		_ForwardShader->setBool("material.hasReflection", true);
	} else {
		Texture::Unbind(GL_TEXTURE_2D);
		_ForwardShader->setBool("material.hasReflection", false);
	}

	Texture::SetActiveUnit(5); //height

	if(_heightMap != nullptr) {
		_heightMap->bind();
		_ForwardShader->setBool("material.hasHeight", true);
	} else {
		Texture::Unbind(GL_TEXTURE_2D);
		_ForwardShader->setBool("material.hasHeight", false);
	}

	//set material properties
	_ForwardShader->setFloat("material.shininess", _shininess);
	_ForwardShader->setFloat("material.refractionFactor", _refractionFactor);
	_ForwardShader->setFloat("material.heightScale", _heightScale);

	_ForwardShader->setInt("material.blendMode", _blendMode);
}

void TextureMaterial::drawDeferred(glm::mat4 & modelMatrix) {
	_DeferredShader->use();

	//set model matrix
	_DeferredShader->setMat4("modelMatrix", modelMatrix);

	//set material textures and bools
	Texture::SetActiveUnit(0); //diffuse

	if(_diffuseMap != nullptr) {
		_diffuseMap->bind();
	} else {
		Texture::Unbind(GL_TEXTURE_2D);
		std::cout << "ERROR: No diffuse map in the texture material. Ensure that there is at least a diffuse map present!" << std::endl;
	}

	Texture::SetActiveUnit(1); //specular

	if(_specularMap != nullptr) {
		_specularMap->bind();
		_DeferredShader->setBool("material.hasSpecular", true);
	} else {
		Texture::Unbind(GL_TEXTURE_2D);
		_DeferredShader->setBool("material.hasSpecular", false);
	}

	Texture::SetActiveUnit(2); //normal

	if(_normalMap != nullptr) {
		_normalMap->bind();
		_DeferredShader->setBool("material.hasNormal", true);
	} else {
		Texture::Unbind(GL_TEXTURE_2D);
		_DeferredShader->setBool("material.hasNormal", false);
	}

	Texture::SetActiveUnit(3); //emission

	if(_emissionMap != nullptr) {
		_emissionMap->bind();
	} else {
		Texture::Unbind(GL_TEXTURE_2D);
	}

	Texture::SetActiveUnit(4); //reflection

	if(_reflectionMap != nullptr) {
		_reflectionMap->bind();
		_DeferredShader->setBool("material.hasReflection", true);
	} else {
		Texture::Unbind(GL_TEXTURE_2D);
		_DeferredShader->setBool("material.hasReflection", false);
	}

	Texture::SetActiveUnit(5); //height

	if(_heightMap != nullptr) {
		_heightMap->bind();
		_DeferredShader->setBool("material.hasHeight", true);
	} else {
		Texture::Unbind(GL_TEXTURE_2D);
		_DeferredShader->setBool("material.hasHeight", false);
	}

	//set material properties
	_DeferredShader->setFloat("material.shininess", _shininess);
	_DeferredShader->setFloat("material.refractionFactor", _refractionFactor);
	_DeferredShader->setFloat("material.heightScale", _heightScale);
}

void TextureMaterial::_initShader() {
	//lazy initialize the shader for all texture materials (there is no need to create one for each material, since they are all the same)
	if(_ForwardShader == nullptr) {
		_ForwardShader = new Shader(Filepath::ShaderPath + "material shader/forward/texture.vs", Filepath::ShaderPath + "material shader/forward/texture.fs");

		_ForwardShader->use();
		_ForwardShader->setInt("material.diffuse", 0);
		_ForwardShader->setInt("material.specular", 1);
		_ForwardShader->setInt("material.normal", 2);
		_ForwardShader->setInt("material.emission", 3);
		_ForwardShader->setInt("material.reflection", 4);
		_ForwardShader->setInt("material.height", 5);

		_ForwardShader->setInt("environmentMap", 8);
		_ForwardShader->setInt("shadowMap", 11); //assign to slot 11, so that it shares it with the other materials which have more textures

		for(unsigned int i = 0; i < RenderSettings::MaxCubeShadows; i++) {
			_ForwardShader->setInt("shadowCubemaps[" + std::to_string(i) + "]", 12 + i);
		}

		_ForwardShader->setUniformBlockBinding("matricesBlock", 0); //set uniform block "matrices" to binding point 0
		_ForwardShader->setUniformBlockBinding("dataBlock", 1); //set uniform block "data" to binding point 1

		_ForwardShader->setShaderStorageBlockBinding("lightsBlock", 2); //set shader storage block "lights" to binding point 2
	}

	if(_DeferredShader == nullptr) {
		_DeferredShader = new Shader(Filepath::ShaderPath + "material shader/deferred/texture.vs", Filepath::ShaderPath + "material shader/deferred/texture.fs");

		_DeferredShader->use();
		_DeferredShader->setInt("material.diffuse", 0);
		_DeferredShader->setInt("material.specular", 1);
		_DeferredShader->setInt("material.normal", 2);
		_DeferredShader->setInt("material.emission", 3);
		_DeferredShader->setInt("material.reflection", 4);
		_DeferredShader->setInt("material.height", 5);

		_DeferredShader->setInt("environmentMap", 8);

		_DeferredShader->setUniformBlockBinding("matricesBlock", 0); //set uniform block "matrices" to binding point 0
		_DeferredShader->setUniformBlockBinding("dataBlock", 1); //set uniform block "data" to binding point 1
	}
}