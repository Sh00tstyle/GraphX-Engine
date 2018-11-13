#include "Material.h"

Material::~Material() {
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

Material::Material(BlendMode blendMode, bool castsShadows): _blendMode(blendMode), _castsShadows(castsShadows) {
}
