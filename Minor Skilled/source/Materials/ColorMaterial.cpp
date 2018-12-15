#include "ColorMaterial.h"

#include <glm/glm.hpp>

#include "../Engine/Shader.h"

#include "../Components/LightComponent.h"

#include "../Utility/Filepath.h"
#include "../Utility/RenderSettings.h"

Shader* ColorMaterial::_ForwardShader = nullptr;
Shader* ColorMaterial::_DeferredShader = nullptr;

ColorMaterial::ColorMaterial(glm::vec3 diffuseColor):Material(MaterialType::Color, BlendMode::Opaque, true), _ambientColor(glm::vec3(0.0f)), _diffuseColor(diffuseColor), _specularColor(glm::vec3(0.0f)), _shininess(32.0f) {
	_initShader();
}

ColorMaterial::ColorMaterial(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, float shininess, bool castsShadows):Material(MaterialType::Color, BlendMode::Opaque, castsShadows),
_ambientColor(ambientColor), _diffuseColor(diffuseColor), _specularColor(specularColor), _shininess(shininess) {
	_initShader();
}

ColorMaterial::~ColorMaterial() {
}

glm::vec3& ColorMaterial::getAmbientColor() {
	return _ambientColor;
}

glm::vec3& ColorMaterial::getDiffuseColor() {
	return _diffuseColor;
}

glm::vec3& ColorMaterial::getSpecularColor() {
	return _specularColor;
}

float& ColorMaterial::getShininess() {
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

void ColorMaterial::drawSimple(Shader* shader) {
	//set environment shader properties
	shader->setBool("useTexture", false);
	shader->setVec3("diffuseVec", _diffuseColor);
}

void ColorMaterial::drawForward(glm::mat4& modelMatrix) {
	_ForwardShader->use();

	//set model matrix
	_ForwardShader->setMat4("modelMatrix", modelMatrix);

	//set material properties
	_ForwardShader->setVec3("material.diffuse", _diffuseColor);
	_ForwardShader->setVec3("material.ambient", _ambientColor);
	_ForwardShader->setVec3("material.specular", _specularColor);
	_ForwardShader->setFloat("material.shininess", _shininess);
}

void ColorMaterial::drawDeferred(glm::mat4 & modelMatrix) {
	_DeferredShader->use();

	//set model matrix
	_DeferredShader->setMat4("modelMatrix", modelMatrix);

	//set material properties
	_DeferredShader->setVec3("material.diffuse", _diffuseColor);
	_DeferredShader->setVec3("material.ambient", _ambientColor);
	_DeferredShader->setVec3("material.specular", _specularColor);
	_DeferredShader->setFloat("material.shininess", _shininess);
}

void ColorMaterial::_initShader() {
	if(_ForwardShader == nullptr) {
		_ForwardShader = new Shader(Filepath::ShaderPath + "material shader/forward/color.vs", Filepath::ShaderPath + "material shader/forward/color.fs");

		_ForwardShader->use();
		_ForwardShader->setInt("shadowMap", 11); //assign to slot 10, so that it shares it with the other materials which have more textures

		for(unsigned int i = 0; i < RenderSettings::MaxCubeShadows; i++) {
			_ForwardShader->setInt("shadowCubemaps[" + std::to_string(i) + "]", 12 + i);
		}

		_ForwardShader->setUniformBlockBinding("matricesBlock", 0); //set uniform block "matrices" to binding point 0
		_ForwardShader->setUniformBlockBinding("dataBlock", 1); //set uniform block "data" to binding point 1

		_ForwardShader->setShaderStorageBlockBinding("lightsBlock", 2); //set shader storage block "lights" to binding point 2
	}

	if(_DeferredShader == nullptr) {
		_DeferredShader = new Shader(Filepath::ShaderPath + "material shader/deferred/color.vs", Filepath::ShaderPath + "material shader/deferred/color.fs");

		_DeferredShader->use();

		_DeferredShader->setUniformBlockBinding("matricesBlock", 0); //set uniform block "matrices" to binding point 0
		_DeferredShader->setUniformBlockBinding("dataBlock", 1); //set uniform block "data" to binding point 1
	}
}
