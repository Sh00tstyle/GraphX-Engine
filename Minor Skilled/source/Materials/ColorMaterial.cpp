#include "ColorMaterial.h"

#include "../Engine/Shader.h"
#include "../Utility/Filepath.h"

Shader* ColorMaterial::_shader = nullptr;

ColorMaterial::ColorMaterial(glm::vec3 color):Material(), _color(color) {
	if(_shader == nullptr) {
		_shader = new Shader(Filepath::ShaderPath + "materials/color.vs", Filepath::ShaderPath + "materials/color.fs");
	}
}

ColorMaterial::~ColorMaterial() {
}

glm::vec3 ColorMaterial::getColor() {
	return _color;
}

void ColorMaterial::setColor(glm::vec3 color) {
	_color = color;
}

void ColorMaterial::draw(glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
	_shader->use();
	_shader->setVec3("color", _color);
	_shader->setMat4("modelMatrix", modelMatrix);
	_shader->setMat4("viewMatrix", viewMatrix);
	_shader->setMat4("projectionMatrix", projectionMatrix);
}