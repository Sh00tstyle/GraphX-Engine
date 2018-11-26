#include "TextureMaterial.h"

#include <glm/glm.hpp>

#include "../Engine/Texture.h"
#include "../Engine/Shader.h"

#include "../Components/LightComponent.h"

#include "../Utility/Filepath.h"

Shader* TextureMaterial::_ForwardShader = nullptr;
Shader* TextureMaterial::_DeferredShader = nullptr;

TextureMaterial::TextureMaterial(Texture* diffuseMap, float shininess) :Material(BlendMode::Opaque, true), _diffuseMap(diffuseMap), _specularMap(nullptr),
_normalMap(nullptr), _emissionMap(nullptr), _heightMap(nullptr), _shininess(shininess), _heightScale(1.0f) {
	_initShader();
}

TextureMaterial::TextureMaterial(Texture * diffuseMap, Texture * specularMap, Texture * normalMap, Texture* emissionMap, Texture* heightMap, float shininess, float heightScale, BlendMode blendMode, bool castsShadows) :
	Material(blendMode, castsShadows),_diffuseMap(diffuseMap), _specularMap(specularMap), _normalMap(normalMap), _heightMap(heightMap), _emissionMap(emissionMap), _shininess(shininess), _heightScale(heightScale) {
	_initShader();
}

TextureMaterial::~TextureMaterial() {
	delete _diffuseMap;
	delete _specularMap;
	delete _normalMap;
	delete _emissionMap;
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

float TextureMaterial::getShininess() {
	return _shininess;
}

float TextureMaterial::getHeightScale() {
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

void TextureMaterial::setShininess(float shininess) {
	_shininess = shininess;
}

void TextureMaterial::setHeightScale(float heightScale) {
	_heightScale = heightScale;
}

void TextureMaterial::drawForward(glm::mat4& modelMatrix) {
	_ForwardShader->use();

	//set model matrix
	_ForwardShader->setMat4("modelMatrix", modelMatrix);

	//set material textures and bools
	glActiveTexture(GL_TEXTURE0); //diffuse

	if(_diffuseMap != nullptr) {
		glBindTexture(GL_TEXTURE_2D, _diffuseMap->getID());
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		std::cout << "ERROR: No diffuse map in the texture material. Ensure that there is at least a diffuse map present!" << std::endl;
	}
	
	glActiveTexture(GL_TEXTURE1); //specular

	if(_specularMap != nullptr) {
		glBindTexture(GL_TEXTURE_2D, _specularMap->getID());
		_ForwardShader->setBool("material.hasSpecular", true);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		_ForwardShader->setBool("material.hasSpecular", false);
	}

	glActiveTexture(GL_TEXTURE2); //normal

	if(_normalMap != nullptr) {
		glBindTexture(GL_TEXTURE_2D, _normalMap->getID());
		_ForwardShader->setBool("material.hasNormal", true);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		_ForwardShader->setBool("material.hasNormal", false);
	}

	glActiveTexture(GL_TEXTURE3); //emission

	if(_emissionMap != nullptr) {
		glBindTexture(GL_TEXTURE_2D, _emissionMap->getID());
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glActiveTexture(GL_TEXTURE4); //height

	if(_heightMap != nullptr) {
		glBindTexture(GL_TEXTURE_2D, _heightMap->getID());
		_ForwardShader->setBool("material.hasHeight", true);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		_ForwardShader->setBool("material.hasHeight", false);
	}

	//set material properties
	_ForwardShader->setFloat("material.shininess", _shininess);
	_ForwardShader->setFloat("material.heightScale", _heightScale);
	_ForwardShader->setInt("material.blendMode", _blendMode);
}

void TextureMaterial::drawDeferred(glm::mat4 & modelMatrix) {
	_DeferredShader->use();

	//set model matrix
	_DeferredShader->setMat4("modelMatrix", modelMatrix);

	//set material textures and bools
	glActiveTexture(GL_TEXTURE0); //diffuse

	if(_diffuseMap != nullptr) {
		glBindTexture(GL_TEXTURE_2D, _diffuseMap->getID());
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		std::cout << "ERROR: No diffuse map in the texture material. Ensure that there is at least a diffuse map present!" << std::endl;
	}

	glActiveTexture(GL_TEXTURE1); //specular

	if(_specularMap != nullptr) {
		glBindTexture(GL_TEXTURE_2D, _specularMap->getID());
		_ForwardShader->setBool("material.hasSpecular", true);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		_ForwardShader->setBool("material.hasSpecular", false);
	}

	glActiveTexture(GL_TEXTURE2); //normal

	if(_normalMap != nullptr) {
		glBindTexture(GL_TEXTURE_2D, _normalMap->getID());
		_ForwardShader->setBool("material.hasNormal", true);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		_ForwardShader->setBool("material.hasNormal", false);
	}

	glActiveTexture(GL_TEXTURE3); //emission

	if(_emissionMap != nullptr) {
		glBindTexture(GL_TEXTURE_2D, _emissionMap->getID());
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glActiveTexture(GL_TEXTURE4); //height

	if(_heightMap != nullptr) {
		glBindTexture(GL_TEXTURE_2D, _heightMap->getID());
		_ForwardShader->setBool("material.hasHeight", true);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		_ForwardShader->setBool("material.hasHeight", false);
	}

	//set material properties
	_DeferredShader->setFloat("material.shininess", _shininess);
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
		_ForwardShader->setInt("material.height", 4);

		_ForwardShader->setInt("shadowMap", 8); //assign to slot 8, so that it shares it with the other materials which have more textures

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
		_DeferredShader->setInt("material.height", 4);

		_DeferredShader->setUniformBlockBinding("matricesBlock", 0); //set uniform block "matrices" to binding point 0
	}
}