#include "ColorMaterial.h"

#include <glm/glm.hpp>

#include "../Engine/Shader.h"

#include "../Components/LightComponent.h"

#include "../Utility/Filepath.h"

Shader* ColorMaterial::_shader = nullptr;

ColorMaterial::ColorMaterial():Material(), _ambientColor(glm::vec3(0.0f)), _diffuseColor(glm::vec3(0.0f)), _specularColor(glm::vec3(0.0f)), _shininess(32.0f) {
	if(_shader == nullptr) {
		_shader = new Shader(Filepath::ShaderPath + "material shader/color.vs", Filepath::ShaderPath + "material shader/color.fs");
	}
}

ColorMaterial::ColorMaterial(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, float shininess):Material(),
_ambientColor(ambientColor), _diffuseColor(diffuseColor), _specularColor(specularColor), _shininess(shininess) {
	if(_shader == nullptr) {
		_shader = new Shader(Filepath::ShaderPath + "material shader/color.vs", Filepath::ShaderPath + "material shader/color.fs");
	}
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

void ColorMaterial::draw(glm::mat4& modelMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix, glm::vec3& cameraPos, std::map<LightComponent*, glm::vec3>& lights) {
	_shader->use();

	//set mvp matrix
	_shader->setMat4("modelMatrix", modelMatrix);
	_shader->setMat4("viewMatrix", viewMatrix);
	_shader->setMat4("projectionMatrix", projectionMatrix);

	//set material properties
	_shader->setVec3("material.diffuse", _diffuseColor);
	_shader->setVec3("material.ambient", _ambientColor);
	_shader->setVec3("material.specular", _specularColor);
	_shader->setFloat("material.shininess", _shininess);

	//set camera pos
	_shader->setVec3("cameraPos", cameraPos);

	//set lights (should only be done here in forward rendering)
	unsigned int counter = 0;
	LightComponent* currentLight;

	for(std::map<LightComponent*, glm::vec3>::iterator it = lights.begin(); it != lights.end(); it++) {
		currentLight = it->first;

		//set light properties
		_shader->setInt("lights[" + std::to_string(counter) + "].type", currentLight->lightType);

		_shader->setVec3("lights[" + std::to_string(counter) + "].position", it->second);
		_shader->setVec3("lights[" + std::to_string(counter) + "].direction", currentLight->lightDirection);

		_shader->setVec3("lights[" + std::to_string(counter) + "].diffuse", currentLight->lightDiffuse);
		_shader->setVec3("lights[" + std::to_string(counter) + "].ambient", currentLight->lightAmbient);
		_shader->setVec3("lights[" + std::to_string(counter) + "].specular", currentLight->lightSpecular);

		_shader->setFloat("lights[" + std::to_string(counter) + "].constantAttenuation", currentLight->constantAttenuation);
		_shader->setFloat("lights[" + std::to_string(counter) + "].linearAttenuation", currentLight->linearAttenuation);
		_shader->setFloat("lights[" + std::to_string(counter) + "].quadraticAttentuation", currentLight->quadraticAttenuation);
		_shader->setFloat("lights[" + std::to_string(counter) + "].innerCutoff", currentLight->innerCutoff);
		_shader->setFloat("lights[" + std::to_string(counter) + "].outerCutoff", currentLight->outerCutoff);

		counter++;

		if(counter >= 10) break; //right now the light array is capped to a maximum of 10 lights
	}
}