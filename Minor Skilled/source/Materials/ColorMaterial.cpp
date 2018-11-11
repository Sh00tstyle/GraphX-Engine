#include "ColorMaterial.h"

#include <glm/glm.hpp>

#include "../Engine/Shader.h"

#include "../Components/LightComponent.h"

#include "../Utility/Filepath.h"

Shader* ColorMaterial::_Shader = nullptr;

ColorMaterial::ColorMaterial():Material(BlendMode::Opaque), _ambientColor(glm::vec3(0.0f)), _diffuseColor(glm::vec3(0.0f)), _specularColor(glm::vec3(0.0f)), _shininess(32.0f) {
	if(_Shader == nullptr) {
		_Shader = new Shader(Filepath::ShaderPath + "material shader/color.vs", Filepath::ShaderPath + "material shader/color.fs");

		_Shader->use();
		_Shader->setInt("shadowMap", 8); //assign to slot 8, so that it shares it with the other materials which have more textures
	}
}

ColorMaterial::ColorMaterial(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, float shininess):Material(BlendMode::Opaque),
_ambientColor(ambientColor), _diffuseColor(diffuseColor), _specularColor(specularColor), _shininess(shininess) {
	if(_Shader == nullptr) {
		_Shader = new Shader(Filepath::ShaderPath + "material shader/color.vs", Filepath::ShaderPath + "material shader/color.fs");

		_Shader->use();
		_Shader->setInt("shadowMap", 8); //assign to slot 8, so that it shares it with the other materials which have more textures
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

void ColorMaterial::draw(glm::mat4& modelMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix, glm::mat4& lightSpaceMatrix, glm::vec3& cameraPos, glm::vec3& directionalLightPos, std::vector<std::pair<LightComponent*, glm::vec3>>& lights) {
	_Shader->use();

	//set mvp matrix
	_Shader->setMat4("modelMatrix", modelMatrix);
	_Shader->setMat4("viewMatrix", viewMatrix);
	_Shader->setMat4("projectionMatrix", projectionMatrix);
	_Shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	//set material properties
	_Shader->setVec3("material.diffuse", _diffuseColor);
	_Shader->setVec3("material.ambient", _ambientColor);
	_Shader->setVec3("material.specular", _specularColor);
	_Shader->setFloat("material.shininess", _shininess);

	//set camera pos and directional light pos
	_Shader->setVec3("cameraPos", cameraPos);
	_Shader->setVec3("directionalLightPos", directionalLightPos);

	//set lights (should only be done here in forward rendering)
	LightComponent* currentLight;

	for(unsigned int i = 0; i < lights.size(); i++) {
		currentLight = lights[i].first;

		//set light properties
		_Shader->setInt("lights[" + std::to_string(i) + "].type", currentLight->lightType);

		_Shader->setVec3("lights[" + std::to_string(i) + "].position", lights[i].second);
		_Shader->setVec3("lights[" + std::to_string(i) + "].direction", currentLight->lightDirection);

		_Shader->setVec3("lights[" + std::to_string(i) + "].diffuse", currentLight->lightDiffuse);
		_Shader->setVec3("lights[" + std::to_string(i) + "].ambient", currentLight->lightAmbient);
		_Shader->setVec3("lights[" + std::to_string(i) + "].specular", currentLight->lightSpecular);

		_Shader->setFloat("lights[" + std::to_string(i) + "].constant", currentLight->constantAttenuation);
		_Shader->setFloat("lights[" + std::to_string(i) + "].linear", currentLight->linearAttenuation);
		_Shader->setFloat("lights[" + std::to_string(i) + "].quadratic", currentLight->quadraticAttenuation);
		_Shader->setFloat("lights[" + std::to_string(i) + "].innerCutoff", currentLight->innerCutoff);
		_Shader->setFloat("lights[" + std::to_string(i) + "].outerCutoff", currentLight->outerCutoff);

		if(i >= LightComponent::LightAmount) break; //right now the light array is capped to a maximum of 10 lights
	}
}