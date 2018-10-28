#include "TextureMaterial.h"

#include <glm/glm.hpp>

#include "../Engine/Texture.h"
#include "../Engine/Shader.h"

#include "../Components/LightComponent.h"

#include "../Utility/Filepath.h"

Shader* TextureMaterial::_shader = nullptr;

TextureMaterial::TextureMaterial() :Material(), _diffuseMap(nullptr), _specularMap(nullptr), _normalMap(nullptr), _emissionMap(nullptr), _shininess(32.0f) {
}

TextureMaterial::TextureMaterial(Texture * diffuseMap, Texture * specularMap, Texture * normalMap, Texture* emissionMap, float shininess) :Material(),
_diffuseMap(diffuseMap), _specularMap(specularMap), _normalMap(normalMap), _emissionMap(emissionMap), _shininess(shininess) {
	if(_shader == nullptr) {
		_shader = new Shader(Filepath::ShaderPath + "material shader/texture.vs", Filepath::ShaderPath + "material shader/texture.fs");
	}

	_shader->use();
	_shader->setInt("material.diffuse", 0);
	_shader->setInt("material.specular", 1);
	_shader->setInt("material.normal", 2);
	_shader->setInt("material.emission", 3);
}

TextureMaterial::~TextureMaterial() {
	delete _diffuseMap;
	delete _specularMap;
	delete _normalMap;
	delete _emissionMap;
}

TextureMaterial * TextureMaterial::LoadMaterial(std::string path) {
	TextureMaterial* textureMaterial = new TextureMaterial();

	//TODO

	return textureMaterial;
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

Texture * TextureMaterial::getEmissionMap() {
	return _emissionMap;
}

float TextureMaterial::getShininess() {
	return _shininess;
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

void TextureMaterial::setEmissionMap(Texture * emissionMap) {
	_emissionMap = emissionMap;
}

void TextureMaterial::setShininess(float shininess) {
	_shininess = shininess;
}

void TextureMaterial::draw(glm::mat4& modelMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix, glm::vec3& cameraPos, std::map<LightComponent*, glm::vec3>& lights) {
	_shader->use();

	//set mvp matrix
	_shader->setMat4("modelMatrix", modelMatrix);
	_shader->setMat4("viewMatrix", viewMatrix);
	_shader->setMat4("projectionMatrix", projectionMatrix);

	//set material properties
	if(_diffuseMap != nullptr) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _diffuseMap->getID());
	}
	
	if(_specularMap != nullptr) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _specularMap->getID());
	}

	if(_normalMap != nullptr) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, _normalMap->getID());
	}

	if(_emissionMap != nullptr) {
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, _emissionMap->getID());
	}

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