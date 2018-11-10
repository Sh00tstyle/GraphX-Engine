#include "Renderer.h"

#include <iostream>
#include <algorithm>
#include <map>

#include <glad/glad.h> //NOTE: glad needs to the be included BEFORE glfw, throws errors otherwise
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "../Engine/Node.h"
#include "../Engine/Transform.h"
#include "../Engine/Material.h"
#include "../Engine/Model.h"
#include "../Engine/Texture.h"
#include "../Engine/Shader.h"

#include "../Components/LightComponent.h"
#include "../Components/CameraComponent.h"
#include "../Components/RenderComponent.h"

#include "../Utility/Filepath.h"
#include "../Utility/ComponentType.h"

Shader* Renderer::_skyboxShader = nullptr;
const float Renderer::_skyboxVertices[] = {
	//positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f,  1.0f
};

Renderer::Renderer() {
	glEnable(GL_DEPTH_TEST); //enable the z-buffer
	glDepthFunc(GL_LEQUAL); //set depth funtion to lass than AND equal for skybox depth trick

	glEnable(GL_BLEND); //enable blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //set blending function

    //glEnable(GL_STENCIL_TEST); //enable the stencil buffer

	//glEnable(GL_CULL_FACE); //enable face culling
	//glCullFace(GL_BACK); //cull backfaces

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); //enable seamless cubemap sampling for lower mip map levels in the pre filter map

	//lazy initialize skybox shader
	if(_skyboxShader == nullptr) {
		_skyboxShader = new Shader(Filepath::ShaderPath + "skybox/skybox.vs", Filepath::ShaderPath + "skybox/skybox.fs");

		_skyboxShader->use();
		_skyboxShader->setInt("skybox", 0);

		_initSkyboxVAO();
	}
}

Renderer::~Renderer() {
	delete _skyboxShader;
}

void Renderer::render(std::vector<Node*>& renderables, std::vector<Node*>& lights, std::vector<Node*>& cameras, Texture* skybox) {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//crete vectors of pairs to match components to their model matrices/positions (allows duplicates, unlike maps)
	std::vector<std::pair<RenderComponent*, glm::mat4>> renderComponents;
	std::vector<std::pair<CameraComponent*, glm::mat4>> cameraComponents;
	std::vector<std::pair<LightComponent*, glm::vec3>> lightComponents;

	//fill collections with data
	for(unsigned int i = 0; i < renderables.size(); i++) {
		std::pair<RenderComponent*, glm::mat4> renderablePair;
		renderablePair.first = (RenderComponent*)renderables[i]->getComponent(ComponentType::Render);
		renderablePair.second = renderables[i]->getTransform()->worldTransform;

		renderComponents.push_back(renderablePair);
	}

	for(unsigned int i = 0; i < cameras.size(); i++) {
		std::pair<CameraComponent*, glm::mat4> cameraPair;
		cameraPair.first = (CameraComponent*)cameras[i]->getComponent(ComponentType::Camera);
		cameraPair.second = cameras[i]->getTransform()->worldTransform;

		cameraComponents.push_back(cameraPair);
	}

	for(unsigned int i = 0; i < lights.size(); i++) {
		std::pair<LightComponent*, glm::vec3> lightPair;
		lightPair.first = (LightComponent*)lights[i]->getComponent(ComponentType::Light);
		lightPair.second = lights[i]->getTransform()->getWorldPosition();

		lightComponents.push_back(lightPair);
	}

	//render each camera
	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	glm::vec3 cameraPos;

	glm::mat4 modelMatrix;
	Material* material;
	Model* model;

	for(unsigned int i = 0; i < cameraComponents.size(); i++) {
		//get relevant properties from the camera
		projectionMatrix = cameraComponents[i].first->projectionMatrix;
		viewMatrix = glm::inverse(cameraComponents[i].second);
		cameraPos = cameraComponents[i].second[3];

		renderComponents = _getSortedRenderComponents(renderables, cameraPos); //Possible optimization: only re-sort if the camera moved

		//render each renderable
		for(unsigned int j = 0; j < renderComponents.size(); j++) {
			modelMatrix = renderComponents[j].second;
			material = renderComponents[j].first->material;
			model = renderComponents[j].first->model;

			material->draw(modelMatrix, viewMatrix, projectionMatrix, cameraPos, lightComponents);
			model->draw();
		}

		//render skybox
		_renderSkybox(viewMatrix, projectionMatrix, skybox);
	}
}

void Renderer::_initSkyboxVAO() {
	glGenVertexArrays(1, &_skyboxVAO);
	glGenBuffers(1, &_skyboxVBO);
	glBindVertexArray(_skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, _skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_skyboxVertices), &_skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void Renderer::_renderSkybox(glm::mat4& viewMatrix, glm::mat4& projectionMatrix, Texture* skybox) {
	if(skybox == nullptr) return;

	glm::mat4 newViewMatrix = glm::mat4(glm::mat3(viewMatrix)); //remove translation from view matrix

	//set shader uniforms
	_skyboxShader->use();
	_skyboxShader->setMat4("viewMatrix", newViewMatrix);
	_skyboxShader->setMat4("projectionMatrix", projectionMatrix);

	//render skybox
	glBindVertexArray(_skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getID());
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

std::vector<std::pair<RenderComponent*, glm::mat4>> Renderer::_getSortedRenderComponents(std::vector<Node*>& renderables, glm::vec3& cameraPos) {
	//returns a vector of pairs  of render components and their respective model matrix (sorted by Opaque -> distance to camera)

	//fill both vectors with the objects
	std::vector<RenderComponent*> solidObjects;
	std::vector<RenderComponent*> blendObjects;
	RenderComponent* renderComponent;
	Material* material;

	for(unsigned int i = 0; i < renderables.size(); i++) {
		renderComponent = (RenderComponent*)renderables[i]->getComponent(ComponentType::Render);
		material = renderComponent->material;

		if(material->getBlendMode() == BlendMode::Opaque) {
			solidObjects.push_back(renderComponent);
		} else {
			blendObjects.push_back(renderComponent);
		}
	}

	//sort the blend objects based on the camera position
	std::map<float, RenderComponent*> sortedBlendObjects; //using a map, since it automatically sorts its entires by key
	glm::vec3 objectPos;

	for(unsigned int i = 0; i < blendObjects.size(); i++) {
		objectPos = blendObjects[i]->getOwner()->getTransform()->getWorldPosition();

		float distance = glm::distance2(cameraPos, objectPos); //squared distance, since we are only comparing distances against each other

		while(sortedBlendObjects[distance] != nullptr) { //offset the distance slightly if there is an objects with the exact same distance already to allow having both in the map
			distance += 0.0001f;
		}

		sortedBlendObjects[distance] = blendObjects[i]; //add entry
	}

	//finally fill the vector of sorted solid and blend objects
	std::vector<std::pair<RenderComponent*, glm::mat4>> sortedRenderComponents;

	for(unsigned int i = 0; i < solidObjects.size(); i++) {
		//fill solid objects in first, since they need to  be rendered first
		std::pair<RenderComponent*, glm::mat4> renderPair;
		renderPair.first = solidObjects[i];
		renderPair.second = solidObjects[i]->getOwner()->getTransform()->worldTransform;

		sortedRenderComponents.push_back(renderPair);
	}

	for(std::map<float, RenderComponent*>::iterator it = sortedBlendObjects.begin(); it != sortedBlendObjects.end(); it++) {
		//fill cutout and transparent objects sorted by the distance to the camera last
		std::pair<RenderComponent*, glm::mat4> renderPair;
		renderPair.first = it->second;
		renderPair.second = it->second->getOwner()->getTransform()->worldTransform;

		sortedRenderComponents.push_back(renderPair);
	}

	return sortedRenderComponents; //return the sorted lists
}