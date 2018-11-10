#include "Material.h"

Material::~Material() {
}

BlendMode Material::getBlendMode() {
	return _blendMode;
}

void Material::setBlendMode(BlendMode blendMode) {
	_blendMode = blendMode;
}

Material::Material(BlendMode blendMode): _blendMode(blendMode) {
}
