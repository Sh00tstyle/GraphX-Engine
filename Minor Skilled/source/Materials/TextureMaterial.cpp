#include "TextureMaterial.h"

#include "../Engine/Texture.h"
#include "../Engine/Shader.h"
#include "../Utility/Filepath.h"

#include <iostream>

Shader* TextureMaterial::_shader = nullptr;

TextureMaterial::TextureMaterial(Texture * diffuseMap, Texture * specularMap, Texture * normalMap, Texture* emissionMap) :Material(),
_diffuseMap(diffuseMap), _specularMap(specularMap), _normalMap(normalMap), _emissionMap(emissionMap) {
	if(_shader == nullptr) {
		_shader = new Shader(Filepath::ShaderPath + "materials/texture.vs", Filepath::ShaderPath + "materials/texture.fs");
	}

	_shader->use();
	_shader->setInt("diffuseMap", 0);
	_shader->setInt("specularMap", 1);
	_shader->setInt("normalMap", 2);
	_shader->setInt("emissionMap", 3);
}

TextureMaterial::~TextureMaterial() {
	delete _diffuseMap;
	delete _specularMap;
	delete _normalMap;
	delete _emissionMap;
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

void TextureMaterial::draw(glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
	_shader->use();
	_shader->setMat4("modelMatrix", modelMatrix);
	_shader->setMat4("viewMatrix", viewMatrix);
	_shader->setMat4("projectionMatrix", projectionMatrix);

	if(_diffuseMap != nullptr) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _diffuseMap->getID());
	}
	
	if(_specularMap != nullptr) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _specularMap->getID());
	}

	if(_normalMap != nullptr) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, _normalMap->getID());
	}

	if(_emissionMap != nullptr) {
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, _emissionMap->getID());
	}
}