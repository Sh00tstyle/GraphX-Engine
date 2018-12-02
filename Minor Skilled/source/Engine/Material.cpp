#include "Material.h"

Material::~Material() {
}

MaterialType Material::getMaterialType() {
	return _materialType;
}

BlendMode Material::getBlendMode() {
	return _blendMode;
}

void Material::setBlendMode(BlendMode blendMode) {
	_blendMode = blendMode;
}

void Material::setCastsShadows(bool value) {
	_castsShadows = value;
}

bool Material::getCastsShadows() {
	return _castsShadows;
}

Material::Material(MaterialType materialType, BlendMode blendMode, bool castsShadows): _materialType(materialType), _blendMode(blendMode), _castsShadows(castsShadows) {
}
