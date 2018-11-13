#include "ColorMaterial.h"

#include <glm/glm.hpp>

#include "../Engine/Shader.h"

#include "../Components/LightComponent.h"

#include "../Utility/Filepath.h"

Shader* ColorMaterial::_Shader = nullptr;

ColorMaterial::ColorMaterial():Material(BlendMode::Opaque, true), _ambientColor(glm::vec3(0.0f)), _diffuseColor(glm::vec3(0.0f)), _specularColor(glm::vec3(0.0f)), _shininess(32.0f) {
	_initShader();
}

ColorMaterial::ColorMaterial(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, float shininess, bool castsShadows):Material(BlendMode::Opaque, castsShadows),
_ambientColor(ambientColor), _diffuseColor(diffuseColor), _specularColor(specularColor), _shininess(shininess) {
	_initShader();
}

ColorMaterial::~ColorMaterial() {
}

glm::vec3 ColorMaterial::getAmbientColor() {
	return _ambientColor;
}

glm::vec3 ColorMaterial::getDiffuseColor() {
	return _diffuseColor;
}

glm::vec3 ColorMaterial::getSpecularColor() {
	return _specularColor;
}

float ColorMaterial::getShininess() {
	return _shininess;
}

void ColorMaterial::setAmbientColor(glm::vec3 ambientColor) {
	_ambientColor = ambientColor;
}

void ColorMaterial::setDiffuseColor(glm::vec3 diffuseColor) {
	_diffuseColor = diffuseColor;
}

void ColorMaterial::setSpecularColor(glm::vec3 specularColor) {
	_specularColor = specularColor;
}

void ColorMaterial::setShininess(float shininess) {
	_shininess = shininess;
}

void ColorMaterial::draw(glm::mat4& modelMatrix) {
	_Shader->use();

	//set model matrix
	_Shader->setMat4("modelMatrix", modelMatrix);

	//set material properties
	_Shader->setVec3("material.diffuse", _diffuseColor);
	_Shader->setVec3("material.ambient", _ambientColor);
	_Shader->setVec3("material.specular", _specularColor);
	_Shader->setFloat("material.shininess", _shininess);
}

void ColorMaterial::_initShader() {
	if(_Shader == nullptr) {
		_Shader = new Shader(Filepath::ShaderPath + "material shader/color.vs", Filepath::ShaderPath + "material shader/color.fs");

		_Shader->use();
		_Shader->setInt("shadowMap", 8); //assign to slot 8, so that it shares it with the other materials which have more textures

		_Shader->setUniformBlockBinding("matricesBlock", 0); //set uniform block "matrices" to binding point 0
		_Shader->setUniformBlockBinding("dataBlock", 1); //set uniform block "data" to binding point 1
		_Shader->setUniformBlockBinding("lightsBlock", 2); //set uniform block "lights" to binding point 2
	}
}
