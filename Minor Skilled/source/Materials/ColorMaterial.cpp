#include "ColorMaterial.h"

ColorMaterial::ColorMaterial(glm::vec3 color):Material(), _color(color) {
	//TODO: init shader
}

ColorMaterial::~ColorMaterial() {
}

glm::vec3 ColorMaterial::getColor() {
	return _color;
}

void ColorMaterial::setColor(glm::vec3 color) {
	_color = color;
}

void ColorMaterial::draw() {
	//TODO
}