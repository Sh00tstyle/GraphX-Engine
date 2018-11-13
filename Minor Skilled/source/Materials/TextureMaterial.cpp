#include "TextureMaterial.h"

#include <glm/glm.hpp>

#include "../Engine/Texture.h"
#include "../Engine/Shader.h"

#include "../Components/LightComponent.h"

#include "../Utility/Filepath.h"

Shader* TextureMaterial::_Shader = nullptr;

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

void TextureMaterial::draw(glm::mat4& modelMatrix) {
	_Shader->use();

	//set model matrix
	_Shader->setMat4("modelMatrix", modelMatrix);

	//set material textures and bools
	if(_diffuseMap != nullptr) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _diffuseMap->getID());
	} else {
		std::cout << "ERROR: No diffuse map in the texture material. Ensure that there is at least a diffuse map present!" << std::endl;
	}
	
	if(_specularMap != nullptr) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _specularMap->getID());

		_Shader->setBool("material.hasSpecular", true);
	} else {
		_Shader->setBool("material.hasSpecular", false);
	}

	if(_normalMap != nullptr) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, _normalMap->getID());

		_Shader->setBool("material.hasNormal", true);
	} else {
		_Shader->setBool("material.hasNormal", false);
	}

	if(_emissionMap != nullptr) {
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, _emissionMap->getID());
	}

	if(_heightMap != nullptr) {
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, _heightMap->getID());

		_Shader->setBool("material.hasHeight", true);
	} else {
		_Shader->setBool("material.hasHeight", false);
	}

	//set material properties
	_Shader->setFloat("material.shininess", _shininess);
	_Shader->setFloat("material.heightScale", _heightScale);
	_Shader->setInt("material.blendMode", _blendMode);
}

void TextureMaterial::_initShader() {
	//lazy initialize the shader for all texture materials (there is no need to create one for each material, since they are all the same)
	if(_Shader == nullptr) {
		_Shader = new Shader(Filepath::ShaderPath + "material shader/texture.vs", Filepath::ShaderPath + "material shader/texture.fs");

		_Shader->use();
		_Shader->setInt("material.diffuse", 0);
		_Shader->setInt("material.specular", 1);
		_Shader->setInt("material.normal", 2);
		_Shader->setInt("material.emission", 3);
		_Shader->setInt("material.height", 4);

		_Shader->setInt("shadowMap", 8); //assign to slot 8, so that it shares it with the other materials which have more textures

		_Shader->setUniformBlockBinding("matricesBlock", 0); //set uniform block "matrices" to binding point 0
		_Shader->setUniformBlockBinding("dataBlock", 1); //set uniform block "data" to binding point 1
		_Shader->setUniformBlockBinding("lightsBlock", 2); //set uniform block "lights" to binding point 2
	}
}