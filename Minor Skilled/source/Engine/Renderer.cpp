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
#include "../Engine/Window.h"
#include "../Engine/GLLight.h"

#include "../Components/LightComponent.h"
#include "../Components/CameraComponent.h"
#include "../Components/RenderComponent.h"

#include "../Utility/Filepath.h"
#include "../Utility/ComponentType.h"

std::bitset<8> Renderer::Settings = 0;

const unsigned int Renderer::_ShadowHeight = 1024;
const unsigned int Renderer::_ShadowWidth = 1024;

const float Renderer::_SkyboxVertices[] = {
	//vertices          
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

const float Renderer::_ScreenQuadVertices[] = {
	//vertices         //uv
	-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
};

Renderer::Renderer() {
	glEnable(GL_DEPTH_TEST); //enable the z-buffer
	glDepthFunc(GL_LESS); //set depth funtion to less

	glEnable(GL_BLEND); //enable blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //set blending function

    //glEnable(GL_STENCIL_TEST); //enable the stencil buffer

	//glEnable(GL_CULL_FACE); //enable face culling
	//glCullFace(GL_BACK); //cull backfaces

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); //enable seamless cubemap sampling for lower mip map levels in the pre filter map

	//shaders
	_initShaders();

	//setup VAOs and VBOs
	_initSkyboxVAO();
	_initScreenQuadVAO();

	//setup uniform buffers
	_initUniformBuffers();

	//setup FBOs
	_initShadowFBO();
}

Renderer::~Renderer() {
	//free resources
	delete _shadowShader;
	delete _skyboxShader;
	delete _screenQuadShader;

	delete _shadowMap;

	glDeleteVertexArrays(1, &_skyboxVAO);
	glDeleteBuffers(1, &_skyboxVBO);

	glDeleteVertexArrays(1, &_screenQuadVAO);
	glDeleteBuffers(1, &_screenQuadVBO);

	glDeleteBuffers(1, &_matricesUBO);
	glDeleteBuffers(1, &_positionsUBO);
	glDeleteBuffers(1, &_lightsUBO);

	glDeleteFramebuffers(1, &_shadowFBO);
}

void Renderer::render(std::vector<Node*>& renderables, std::vector<Node*>& lights, Node* mainCamera, Node* directionalLight, Texture* skybox) {
	//render from main camera
	CameraComponent* mainCameraComponent = (CameraComponent*)mainCamera->getComponent(ComponentType::Camera);
	Transform* mainCameraTransform = mainCamera->getTransform();

	glm::mat4 projectionMatrix = mainCameraComponent->projectionMatrix;
	glm::mat4 viewMatrix = glm::inverse(mainCameraTransform->worldTransform);
	glm::vec3 cameraPos = mainCameraTransform->getWorldPosition();

	//create vectors of pairs to match components to their model matrices/positions
	std::vector<std::pair<RenderComponent*, glm::mat4>> renderComponents;
	std::vector<std::pair<LightComponent*, glm::vec3>> lightComponents;

	//fill collections with data
	renderComponents = _getSortedRenderComponents(renderables, cameraPos); //Possible optimization: only re-sort if the camera moved

	for(unsigned int i = 0; i < lights.size(); i++) {
		std::pair<LightComponent*, glm::vec3> lightPair;
		lightPair.first = (LightComponent*)lights[i]->getComponent(ComponentType::Light);
		lightPair.second = lights[i]->getTransform()->getWorldPosition();

		lightComponents.push_back(lightPair);
	}

	//setup light space matrix
	LightComponent* directionalLightComponent = (LightComponent*)directionalLight->getComponent(ComponentType::Light);
	glm::vec3 directionalLightPos = glm::normalize(directionalLightComponent->lightDirection) * -3.0f; //offset light position 3 units in the opposite direction of the light direction

	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
	glm::mat4 lightView = glm::lookAt(directionalLightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	//store the matrices and the vectors in the uniform buffer
	_fillUniformBuffers(viewMatrix, projectionMatrix, lightSpaceMatrix, cameraPos, directionalLightPos, lightComponents);

	//clear screen in light grey
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	//render shadow map
	_renderShadowMap(renderComponents, lightSpaceMatrix);

	//render scene
	_renderScene(renderComponents);

	//render skybox
	_renderSkybox(viewMatrix, projectionMatrix, skybox);

	//render screen quad
	//_renderScreenQuad(_shadowMap);
}

void Renderer::_initShaders() {
	//initialize shadow shader
	_shadowShader = new Shader(Filepath::ShaderPath + "shadow shader/shadow.vs", Filepath::ShaderPath + "shadow shader/shadow.fs");

	//initialize skybox shader
	_skyboxShader = new Shader(Filepath::ShaderPath + "skybox shader/skybox.vs", Filepath::ShaderPath + "skybox shader/skybox.fs");

	_skyboxShader->use();
	_skyboxShader->setInt("skybox", 0);

	//lazy screen quad shader
	_screenQuadShader = new Shader(Filepath::ShaderPath + "post processing shader/screenQuad.vs", Filepath::ShaderPath + "post processing shader/screenQuad.fs");

	_screenQuadShader->use();
	_screenQuadShader->setInt("screenTexture", 0);
}

void Renderer::_initSkyboxVAO() {
	//setup skybox VAO and VBO
	glGenVertexArrays(1, &_skyboxVAO);
	glGenBuffers(1, &_skyboxVBO);
	glBindVertexArray(_skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, _skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_SkyboxVertices), &_SkyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0); //vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void Renderer::_initScreenQuadVAO() {
	//setup screen quad VAO and VBO
	glGenVertexArrays(1, &_screenQuadVAO);
	glGenBuffers(1, &_screenQuadVBO);
	glBindVertexArray(_screenQuadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, _screenQuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_ScreenQuadVertices), &_ScreenQuadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0); //vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1); //uv
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void Renderer::_initUniformBuffers() {
	//calculate needed memory in bytes
	unsigned int neededMemory = sizeof(glm::mat4) * 3; //3x mat4

	//create matrices uniform buffer
	glGenBuffers(1, &_matricesUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, _matricesUBO);
	glBufferData(GL_UNIFORM_BUFFER, neededMemory, NULL, GL_STATIC_DRAW); //allocate the memory, but don't fill it with data yet
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//define the range of the buffer that links to a uniform binding point (binding point = 0)
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, _matricesUBO, 0, neededMemory);

	//create positions uniform buffer
	neededMemory = sizeof(glm::vec4) * 2; //2x vec3, but needs the memory layout of a vec4

	glGenBuffers(1, &_positionsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, _positionsUBO);
	glBufferData(GL_UNIFORM_BUFFER, neededMemory, NULL, GL_STATIC_DRAW);

	glBindBufferRange(GL_UNIFORM_BUFFER, 1, _positionsUBO, 0, neededMemory); //bind to binding point 1

	//create lights uniform buffer
	neededMemory = sizeof(GLLight) * LightComponent::LightAmount; //calculated 104 bytes per light struct + 8 bytes for padding (112 in total)

	glGenBuffers(1, &_lightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, _lightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, neededMemory, NULL, GL_STATIC_DRAW);

	glBindBufferRange(GL_UNIFORM_BUFFER, 2, _lightsUBO, 0, neededMemory); //bind to binding point 2

	//unbind
	glBindBuffer(GL_UNIFORM_BUFFER, 0); 
}

void Renderer::_initShadowFBO() {
	//create shadow texture
	_shadowMap = new Texture();

	glGenTextures(1, &_shadowMap->getID());
	glBindTexture(GL_TEXTURE_2D, _shadowMap->getID());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _ShadowWidth, _ShadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL); //we only need the depth component
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); //results in a shadow value of 0 when outside of the light frustum
	
	//create shadow framebuffer and attach the shadow map to it, so the framebuffer can render to it
	glGenFramebuffers(1, &_shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _shadowMap->getID(), 0);
	glDrawBuffer(GL_NONE); //explicitly tell OpenGL that we are only using the depth attachments and no color attachments, otherwise the FBO will be incomplete
	glReadBuffer(GL_NONE);

	//check for completion
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: Shadow map framebuffer is incomplete." << std::endl;
	}

	//bind back to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
}

void Renderer::_renderShadowMap(std::vector<std::pair<RenderComponent*, glm::mat4>>& renderComponents, glm::mat4& lightSpaceMatrix) {
	//setup shader uniforms
	_shadowShader->use();
	_shadowShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	//adjust viewport and bind to shadow framebuffer
	glViewport(0, 0, _ShadowWidth, _ShadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, _shadowFBO);

	//render scene from the lights perspective
	glClear(GL_DEPTH_BUFFER_BIT);

	glm::mat4 modelMatrix;
	Model* model;

	for(unsigned int i = 0; i < renderComponents.size(); i++) {
		modelMatrix = renderComponents[i].second;
		model = renderComponents[i].first->model;

		_shadowShader->setMat4("modelMatrix", modelMatrix);
		model->draw();
	}

	//reset viewport and bind back to default framebuffer
	glViewport(0, 0, Window::ScreenWidth, Window::ScreenHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::_renderScene(std::vector<std::pair<RenderComponent*, glm::mat4>>& renderComponents) {
	//render each renderable
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//bind shadow map
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, _shadowMap->getID());

	glm::mat4 modelMatrix;
	Material* material;
	Model* model;

	for(unsigned int i = 0; i < renderComponents.size(); i++) {
		modelMatrix = renderComponents[i].second;
		material = renderComponents[i].first->material;
		model = renderComponents[i].first->model;

		material->draw(modelMatrix);
		model->draw();
	}
}

void Renderer::_renderSkybox(glm::mat4& viewMatrix, glm::mat4& projectionMatrix, Texture* skybox) {
	if(skybox == nullptr) {
		std::cout << "Unable to render skybox, there is no skybox defined." << std::endl;
		return;
	}

	glm::mat4 newViewMatrix = glm::mat4(glm::mat3(viewMatrix)); //remove translation from view matrix

	//set shader uniforms
	_skyboxShader->use();
	_skyboxShader->setMat4("viewMatrix", newViewMatrix);
	_skyboxShader->setMat4("projectionMatrix", projectionMatrix);

	//render skybox
	glDepthFunc(GL_LEQUAL); //set depth funtion to less than AND equal for skybox depth trick
	glBindVertexArray(_skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getID());
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); //depth function set back to default
}

void Renderer::_renderScreenQuad(Texture* screenTexture) {
	//render screen quad with passed in texture
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_screenQuadShader->use();
	
	glBindVertexArray(_screenQuadVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screenTexture->getID()); //render shadow map to screen
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Renderer::_fillUniformBuffers(glm::mat4& viewMatrix, glm::mat4& projectionMatrix, glm::mat4& lightSpaceMatrix, glm::vec3& cameraPos, glm::vec3& directionalLightPos, std::vector<std::pair<LightComponent*, glm::vec3>>& lightComponents) {
	//store the matrices in the matrices uniform buffer
	glBindBuffer(GL_UNIFORM_BUFFER, _matricesUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMatrix)); //buffer view matrix
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projectionMatrix)); //buffer projection matrix
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(lightSpaceMatrix)); //buffer light space matrix

	//store the positions in the positions uniform buffer
	glBindBuffer(GL_UNIFORM_BUFFER, _positionsUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4), glm::value_ptr(cameraPos)); //buffer cameraPos
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(directionalLightPos)); //buffer directional light pos

	//store the lights in the lights uniform buffer
	std::pair<LightComponent*, glm::vec3> currentLightPair;
	LightComponent* currentLight;
	glm::vec3 currentLightPos;

	GLint usedLights = lightComponents.size();
	if(usedLights > LightComponent::LightAmount) usedLights = LightComponent::LightAmount; //limit the amount of possible lights

	glBindBuffer(GL_UNIFORM_BUFFER, _lightsUBO);

	for(unsigned int i = 0; i < usedLights; i++) {
		currentLight = lightComponents[i].first;
		currentLightPos = lightComponents[i].second;

		GLLight light = currentLight->toGLLight(currentLightPos); //convert current light component to a light struct GLSL can understand

		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLLight) * i, sizeof(GLLight), &light); //buffer light struct
	}

	glBindBuffer(GL_UNIFORM_BUFFER, 0); //unbind
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