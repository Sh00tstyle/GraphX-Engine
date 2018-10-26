#include "TextureMaterial.h"

#include "../Engine/Texture.h"

TextureMaterial::TextureMaterial(Texture * diffuseMap, Texture * specularMap, Texture * normalMap):Material(), _diffuseMap(diffuseMap), _specularMap(specularMap), _normalMap(normalMap) {
	//TODO: init shader
}

TextureMaterial::~TextureMaterial() {
	delete _diffuseMap;
	delete _specularMap;
	delete _normalMap;
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

void TextureMaterial::setDiffuseMap(Texture* diffuseMap) {
	_diffuseMap = diffuseMap;
}

void TextureMaterial::setSpecularMap(Texture* specularMap) {
	_specularMap = specularMap;
}

void TextureMaterial::setNormalMap(Texture* normalMap) {
	_normalMap = normalMap;
}

void TextureMaterial::draw() {
	//TODO
}
