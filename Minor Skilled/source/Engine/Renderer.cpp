#include "Renderer.h"

#include <iostream>
#include <algorithm>
#include <map>
#include <random>

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
#include "../Engine/VertexArray.h"
#include "../Engine/Buffer.h"
#include "../Engine/Renderbuffer.h"
#include "../Engine/Framebuffer.h"

#include "../Materials/TextureMaterial.h"

#include "../Components/LightComponent.h"
#include "../Components/CameraComponent.h"
#include "../Components/RenderComponent.h"

#include "../Utility/Filepath.h"
#include "../Utility/ComponentType.h"
#include "../Utility/Math.h"
#include "../Utility/RenderSettings.h"

const std::vector<float> Renderer::_SkyboxVertices = {
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

const std::vector<float> Renderer::_ScreenQuadVertices = {
	//vertices          //uv
	-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
};

Renderer::Renderer() {
	glEnable(GL_DEPTH_TEST); //enable the z-buffer
	glDepthFunc(GL_LESS); //set depth funtion to less

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //set blending function

    //glEnable(GL_STENCIL_TEST); //enable the stencil buffer

	//glEnable(GL_CULL_FACE); //enable face culling
	//glCullFace(GL_BACK); //cull backfaces

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); //enable seamless cubemap sampling for lower mip map levels in the pre filter map

	//glEnable(GL_MULTISAMPLE); //enable MSAA (only works in forward rendering and on the default framebuffer)

	//shaders
	_initShaders();

	//setup VAOs and VBOs
	_initSkyboxVAO();
	_initScreenQuadVAO();

	//setup UBOs and SSBOs
	_initUniformBuffers();
	_initShaderStorageBuffers();

	//setup FBOs
	_initGBuffer();

	_initShadowFBO();
	_initShadowCubeFBOs();
	_initEnvironmentCubeFBO();
	_initHdrFBO();

	//setup post processing
	_initBlurFBOs();
	_initSSAOFBOs();

	_generateSSAOKernel();
	_generateNoiseTexture();
}

Renderer::~Renderer() {
	//delete shaders
	delete _lightingShader;
	delete _shadowShader;
	delete _shadowCubeShader;
	delete _environmentShader;
	delete _faceToCubeShader;
	delete _skyboxShader;
	delete _ssaoShader;
	delete _ssaoBlurShader;
	delete _bloomBlurShader;
	delete _postProcessingShader;

	//delete textures
	delete _gPositionRefract;
	delete _gNormalReflect;
	delete _gAlbedoSpec;
	delete _gEmissionShiny;

	delete _shadowMap;

	for(unsigned int i = 0; i < _shadowCubeMaps.size(); i++) {
		delete _shadowCubeMaps[i];
	}

	for(unsigned int i = 0; i < _environmentMapFaces.size(); i++) {
		delete _environmentMapFaces[i];
	}

	delete _environmentMap;
	delete _sceneColorBuffer;
	delete _brightColorBuffer;
	delete _blurColorBuffers[1];
	delete _blurColorBuffers[0];

	delete _ssaoNoiseTexture;
	delete _ssaoColorBuffer;
	delete _ssaoBlurColorBuffer;

	//delete vertex arrays
	delete _skyboxVAO;
	delete _screenQuadVAO;

	//delete buffers
	delete _skyboxVBO;
	delete _screenQuadVBO;

	delete _matricesUBO;
	delete _dataUBO;

	delete _lightsSSBO;

	//delete framebuffers
	delete _gBuffer;
	delete _shadowFBO;

	for(unsigned int i = 0; i < _shadowCubeFBOs.size(); i++) {
		delete _shadowCubeFBOs[i];
	}

	for(unsigned int i = 0; i < _environmentFBOs.size(); i++) {
		delete _environmentFBOs[i];
	}

	delete _environmentCubeFBO;
	delete _hdrFBO;
	delete _bloomFBO;
	delete _bloomBlurFBOs[0]; 
	delete _bloomBlurFBOs[1];
	delete _ssaoFBO;
	delete _ssaoBlurFBO;

	//delete renderbuffers
	delete _gRBO;

	delete _environmentRBO;
	delete _hdrRBO;
}

void Renderer::render(std::vector<Node*>& renderables, std::vector<Node*>& lights, Node* mainCamera, Node* directionalLight, Texture* skybox) {
	//render from main camera
	if(mainCamera == nullptr) {
		std::cout << "ERROR: There is no main camera assigned. Unable to render scene." << std::endl;
		return;
	}

	CameraComponent* mainCameraComponent = (CameraComponent*)mainCamera->getComponent(ComponentType::Camera);
	Transform* mainCameraTransform = mainCamera->getTransform();

	glm::mat4 projectionMatrix = mainCameraComponent->projectionMatrix;
	glm::mat4 viewMatrix = glm::inverse(mainCameraTransform->worldTransform);
	glm::vec3 cameraPos = mainCameraTransform->getWorldPosition();

	//create vectors of pairs to match components to their model matrices/positions
	std::vector<std::pair<RenderComponent*, glm::mat4>> solidRenderComponents;
	std::vector<std::pair<RenderComponent*, glm::mat4>> blendRenderComponents;

	std::vector<std::pair<RenderComponent*, glm::mat4>> renderComponents;
	std::vector<std::pair<LightComponent*, glm::vec3>> lightComponents;

	//fill collections with data
	_getSortedRenderComponents(renderables, cameraPos, solidRenderComponents, blendRenderComponents); //Possible optimization: only re-sort if the camera moved
	renderComponents.insert(renderComponents.end(), solidRenderComponents.begin(), solidRenderComponents.end()); //add solid objects to the total render component list
	renderComponents.insert(renderComponents.end(), blendRenderComponents.begin(), blendRenderComponents.end()); //add blend objects too

	for(unsigned int i = 0; i < lights.size(); i++) {
		std::pair<LightComponent*, glm::vec3> lightPair;
		lightPair.first = (LightComponent*)lights[i]->getComponent(ComponentType::Light);
		lightPair.second = lights[i]->getTransform()->getWorldPosition();

		lightComponents.push_back(lightPair);
	}

	//setup light space matrix
	std::vector<glm::vec3> pointLightPositions;
	unsigned int pointLightCount;
	glm::vec3 directionalLightPos;

	glm::mat4 lightProjection;
	glm::mat4 lightView;
	glm::mat4 lightSpaceMatrix;

	bool useShadows = RenderSettings::IsEnabled(RenderSettings::Shadows);

	if(directionalLight != nullptr) {
		LightComponent* directionalLightComponent = (LightComponent*)directionalLight->getComponent(ComponentType::Light);
		directionalLightPos = glm::normalize(directionalLightComponent->lightDirection) * -4.0f; //offset light position 4 units in the opposite direction of the light direction

		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
		lightView = glm::lookAt(directionalLightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		lightSpaceMatrix = lightProjection * lightView;
	} else {
		useShadows = false;

		std::cout << "WARNING: No directional light assigned. Unable to render shadowmap." << std::endl;
	}

	//get closest point lights
	if(useShadows) pointLightPositions = _getClosestPointLights(cameraPos, lightComponents);
	pointLightCount = pointLightPositions.size();

	//store the matrices and the vectors in the uniform buffer
	_fillUniformBuffers(viewMatrix, projectionMatrix, lightSpaceMatrix, cameraPos, directionalLightPos, useShadows, pointLightPositions);
	_fillShaderStorageBuffers(lightComponents);

	//clear screen in light grey
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	//render shadow map
	if(useShadows) _renderShadowMaps(renderComponents, pointLightPositions, lightSpaceMatrix);

	//render low quality environment map
	if(RenderSettings::IsEnabled(RenderSettings::EnvironmentMapping)) _renderEnvironmentMap(renderComponents, skybox, cameraPos, pointLightCount);

	//render scene
	if(RenderSettings::IsEnabled(RenderSettings::Deferred)) {
		//render the geometry of the scene (deferred shading)
		_renderGeometry(solidRenderComponents);

		if(RenderSettings::IsEnabled(RenderSettings::SSAO)) {
			//render ssao texture (deferred shading)
			_renderSSAO();

			//blur ssao texture (deferred shading)
			_renderSSAOBlur();
		}

		//render lighting of the scene (deferred shading)
		_renderLighting(skybox, pointLightCount);

		//blit gBuffer depth buffer into the hdr framebuffer depth buffer to enable forward rendering into the deferred scene
		_blitGDepthToHDR();
	} else {
		//render and light the scene (forward shading)
		_renderScene(solidRenderComponents, skybox, pointLightCount, useShadows, true); //bind the hdr here and clear buffer bits
	}

	//render skybox
	_renderSkybox(viewMatrix, projectionMatrix, skybox);

	//render blend objects (forward shading)
	glEnable(GL_BLEND);
	_renderScene(blendRenderComponents, skybox, pointLightCount, useShadows, false); //do not bind the hbr here and clear buffer bits since the solid objects are needed in the fbo
	glDisable(GL_BLEND);

	//render screen quad
	_renderPostProcessingQuad();
}

void Renderer::_initShaders() {
	//initialize lighting pass shader
	_lightingShader = new Shader(Filepath::ShaderPath + "post processing shader/screenQuad.vs", Filepath::ShaderPath + "post processing shader/lightingPass.fs");

	_lightingShader->use();
	_lightingShader->setInt("gPositionRefract", 0);
	_lightingShader->setInt("gNormalReflect", 1);
	_lightingShader->setInt("gAlbedoSpec", 2);
	_lightingShader->setInt("gEmissionShiny", 3);
	_lightingShader->setInt("ssao", 4);

	_lightingShader->setInt("environmentMap", 7);
	_lightingShader->setInt("shadowMap", 8);

	for(unsigned int i = 0; i < RenderSettings::MaxCubeShadows; i++) {
		_lightingShader->setInt("shadowCubemaps[" + std::to_string(i) + "]", 9 + i);
	}

	_lightingShader->setUniformBlockBinding("matricesBlock", 0); //set uniform block "matrices" to binding point 0
	_lightingShader->setUniformBlockBinding("dataBlock", 1); //set uniform block "data" to binding point 1

	_lightingShader->setShaderStorageBlockBinding("lightsBlock", 2); //set shader storage block "lights" to binding point 2

	//initialize shadow shader
	_shadowShader = new Shader(Filepath::ShaderPath + "shadow shader/shadow.vs", Filepath::ShaderPath + "shadow shader/shadow.fs");

	//initialize shadow cubemap shader (including geometry shader for layered rendering)
	_shadowCubeShader = new Shader(Filepath::ShaderPath + "shadow shader/shadowCube.vs", Filepath::ShaderPath + "shadow shader/shadowCube.gs", Filepath::ShaderPath + "shadow shader/shadowCube.fs");

	//initialize environment shader
	_environmentShader = new Shader(Filepath::ShaderPath + "skybox shader/environment.vs", Filepath::ShaderPath + "skybox shader/environment.fs");

	_environmentShader->use();
	_environmentShader->setInt("diffuseMap", 0);
	_environmentShader->setInt("shadowMap", 1);

	_environmentShader->setUniformBlockBinding("matricesBlock", 0);
	_environmentShader->setUniformBlockBinding("dataBlock", 1);

	_environmentShader->setShaderStorageBlockBinding("lightsBlock", 2);

	//initialize face to cubemap shader
	_faceToCubeShader = new Shader(Filepath::ShaderPath + "skybox shader/faceToCube.vs", Filepath::ShaderPath + "skybox shader/faceToCube.fs");

	_faceToCubeShader->use();

	for(unsigned int i = 0; i < 6; i++) {
		_faceToCubeShader->setInt("cubemapFaces[" + std::to_string(i) + "]", i);
	}

	//initialize skybox shader
	_skyboxShader = new Shader(Filepath::ShaderPath + "skybox shader/skybox.vs", Filepath::ShaderPath + "skybox shader/skybox.fs");

	_skyboxShader->use();
	_skyboxShader->setInt("skybox", 0);

	//initialize ssao shader
	_ssaoShader = new Shader(Filepath::ShaderPath + "post processing shader/screenQuad.vs", Filepath::ShaderPath + "post processing shader/ssao.fs");

	_ssaoShader->use();
	_ssaoShader->setInt("gPositionRefract", 0);
	_ssaoShader->setInt("gNormalReflect", 1);
	_ssaoShader->setInt("noiseTexture", 2);

	_ssaoShader->setUniformBlockBinding("matricesBlock", 0); //set uniform block "matrices" to binding point 0

	//initialize ssao blur shader
	_ssaoBlurShader = new Shader(Filepath::ShaderPath + "post processing shader/screenQuad.vs", Filepath::ShaderPath + "post processing shader/ssaoBlur.fs");

	_ssaoBlurShader->use();
	_ssaoBlurShader->setInt("ssaoInput", 0);

	//initialize bloom blur shader
	_bloomBlurShader = new Shader(Filepath::ShaderPath + "post processing shader/screenQuad.vs", Filepath::ShaderPath + "post processing shader/bloomBlur.fs");

	_bloomBlurShader->use();
	_bloomBlurShader->setInt("image", 0);

	//initialize post processing shader
	_postProcessingShader = new Shader(Filepath::ShaderPath + "post processing shader/screenQuad.vs", Filepath::ShaderPath + "post processing shader/postProcessing.fs");

	_postProcessingShader->use();
	_postProcessingShader->setInt("screenTexture", 0);
	_postProcessingShader->setInt("bloomBlur", 1);
}

void Renderer::_initSkyboxVAO() {
	//setup skybox VAO and VBO
	_skyboxVAO = new VertexArray();
	_skyboxVAO->bind();

	_skyboxVBO = new Buffer();
	_skyboxVBO->bind(GL_ARRAY_BUFFER);
	_skyboxVBO->bufferData(GL_ARRAY_BUFFER, &_SkyboxVertices[0], _SkyboxVertices.size() * sizeof(float));

	glEnableVertexAttribArray(0); //vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void Renderer::_initScreenQuadVAO() {
	//setup screen quad VAO and VBO
	_screenQuadVAO = new VertexArray();
	_screenQuadVAO->bind();

	_screenQuadVBO = new Buffer();
	_screenQuadVBO->bind(GL_ARRAY_BUFFER);
	_screenQuadVBO->bufferData(GL_ARRAY_BUFFER, &_ScreenQuadVertices[0], _ScreenQuadVertices.size() * sizeof(float));

	glEnableVertexAttribArray(0); //vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1); //uv
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void Renderer::_initUniformBuffers() {
	//calculate needed memory in bytes
	unsigned int neededMemory = sizeof(glm::mat4) * 3; //3x mat4

	//create matrices uniform buffer
	_matricesUBO = new Buffer();
	_matricesUBO->bind(GL_UNIFORM_BUFFER);
	_matricesUBO->allocateMemory(GL_UNIFORM_BUFFER, neededMemory); //allocate the memory, but don't fill it with data yet
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//define the range of the buffer that links to a uniform binding point (binding point = 0)
	_matricesUBO->bindBufferRange(GL_UNIFORM_BUFFER, 0, neededMemory);

	//create data uniform buffer
	neededMemory = sizeof(glm::vec4) * 3 + sizeof(glm::vec4) * RenderSettings::MaxCubeShadows;

	_dataUBO = new Buffer();
	_dataUBO->bind(GL_UNIFORM_BUFFER);
	_dataUBO->allocateMemory(GL_UNIFORM_BUFFER, neededMemory); //allocate the memory, but don't fill it with data yet

	_dataUBO->bindBufferRange(GL_UNIFORM_BUFFER, 1, neededMemory); //bind to binding point 1

	//unbind
	glBindBuffer(GL_UNIFORM_BUFFER, 0); 
}

void Renderer::_initShaderStorageBuffers() {
	//create lights shader storage buffer (enables use of dynamic arrays within shaders)
	unsigned int neededMemory = sizeof(glm::vec4) + sizeof(GLLight) * RenderSettings::MaxLights; //112 bytes per light struct + 16 for an additional int

	_lightsSSBO = new Buffer();
	_lightsSSBO->bind(GL_SHADER_STORAGE_BUFFER);
	_lightsSSBO->allocateMemory(GL_SHADER_STORAGE_BUFFER, neededMemory); //allocate the memory, but don't fill it with data yet

	_lightsSSBO->bindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, neededMemory); //bind to binding point 2

	//unbind
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::_initGBuffer() {
	//create the position color buffer
	_gPositionRefract = new Texture();
	_gPositionRefract->bind(GL_TEXTURE_2D);
	_gPositionRefract->init(GL_TEXTURE_2D, GL_RGBA16F, Window::ScreenWidth, Window::ScreenHeight, GL_RGBA, GL_FLOAT); //16-bit precision RGBA texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //ensure we don't accidentally oversample
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//create the normal + reflection color buffer
	_gNormalReflect = new Texture();
	_gNormalReflect->bind(GL_TEXTURE_2D);
	_gNormalReflect->init(GL_TEXTURE_2D, GL_RGBA16F, Window::ScreenWidth, Window::ScreenHeight, GL_RGBA, GL_FLOAT); //16-bit precision RGBA texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//create the color + specular color buffer
	_gAlbedoSpec = new Texture();
	_gAlbedoSpec->bind(GL_TEXTURE_2D);
	_gAlbedoSpec->init(GL_TEXTURE_2D, GL_RGBA, Window::ScreenWidth, Window::ScreenHeight, GL_RGBA, GL_UNSIGNED_BYTE); //8-bit precision RGBA texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//create the emission + shininess color buffer
	_gEmissionShiny = new Texture();
	_gEmissionShiny->bind(GL_TEXTURE_2D);
	_gEmissionShiny->init(GL_TEXTURE_2D, GL_RGBA, Window::ScreenWidth, Window::ScreenHeight, GL_RGBA, GL_UNSIGNED_BYTE); //8-bit precision RGBA texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//create depth renderbuffer
	_gRBO = new Renderbuffer();
	_gRBO->bind();
	_gRBO->init(GL_DEPTH_COMPONENT, Window::ScreenWidth, Window::ScreenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//create the gBuffer framebuffer and attach its color buffers for the deferred shading geometry pass
	_gBuffer = new Framebuffer();
	_gBuffer->bind(GL_FRAMEBUFFER);
	_gBuffer->attachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gPositionRefract);
	_gBuffer->attachTexture(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _gNormalReflect);
	_gBuffer->attachTexture(GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _gAlbedoSpec);
	_gBuffer->attachTexture(GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, _gEmissionShiny);
	_gBuffer->attachRenderbuffer(GL_DEPTH_ATTACHMENT, _gRBO);

	//tell OpenGL which attachments the gBuffer will use for rendering
	unsigned int attachments[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
	glDrawBuffers(4, attachments);

	//check for completion
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: gBuffer framebuffer is incomplete." << std::endl;
	}

	//unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::_initShadowFBO() {
	//create shadow texture
	_shadowMap = new Texture();
	_shadowMap->bind(GL_TEXTURE_2D);
	_shadowMap->init(GL_TEXTURE_2D, GL_DEPTH_COMPONENT, RenderSettings::ShadowWidth, RenderSettings::ShadowHeight, GL_DEPTH_COMPONENT, GL_FLOAT); //we only need the depth component

	//set texture filter options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); //results in a shadow value of 0 when outside of the light frustum
	
	//create shadow framebuffer and attach the shadow map to it, so the framebuffer can render to it
	_shadowFBO = new Framebuffer();
	_shadowFBO->bind(GL_FRAMEBUFFER);
	_shadowFBO->attachTexture(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _shadowMap);
	glDrawBuffer(GL_NONE); //explicitly tell OpenGL that we are only using the depth attachments and no color attachments, otherwise the FBO will be incomplete
	glReadBuffer(GL_NONE);

	//check for completion
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: Shadow map framebuffer is incomplete." << std::endl;
	}

	//bind back to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
}

void Renderer::_initShadowCubeFBOs() {
	//create shadow cubemap texture for each possible light
	for(unsigned int i = 0; i < RenderSettings::MaxCubeShadows; i++) {
		_shadowCubeMaps.push_back(new Texture()); //add to vector

		_shadowCubeMaps[i]->bind(GL_TEXTURE_CUBE_MAP);

		//init all cubemap faces
		for(unsigned int j = 0; j < 6; j++) {
			_shadowCubeMaps[i]->init(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, GL_DEPTH_COMPONENT, RenderSettings::ShadowWidth, RenderSettings::ShadowHeight, GL_DEPTH_COMPONENT, GL_FLOAT); //we only need the depth component
		}

		//set texture filter options
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		//create shadow framebuffer and attach the shadow map to it, so the framebuffer can render to it
		_shadowCubeFBOs.push_back(new Framebuffer());

		_shadowCubeFBOs[i]->bind(GL_FRAMEBUFFER);
		_shadowCubeFBOs[i]->attachCubemap(GL_DEPTH_ATTACHMENT, _shadowCubeMaps[i]); //attach texture
		glDrawBuffer(GL_NONE); //explicitly tell OpenGL that we are only using the depth attachments and no color attachments, otherwise the FBO will be incomplete
		glReadBuffer(GL_NONE);

		//check for completion
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "ERROR: Shadow cubemap " + std::to_string(i) + " framebuffer is incomplete." << std::endl;
		}
	}

	//bind back to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::_initEnvironmentCubeFBO() {
	//create renderbuffer to be used by all faces
	_environmentRBO = new Renderbuffer();
	_environmentRBO->bind();
	_environmentRBO->init(GL_DEPTH_COMPONENT, RenderSettings::EnvironmentWidth, RenderSettings::EnvironmentHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//setup framebuffer for each cubemap face
	for(unsigned int i = 0; i < 6; i++) {
		//create texture color buffer for the face
		_environmentMapFaces.push_back(new Texture());
		_environmentMapFaces[i]->bind(GL_TEXTURE_2D);
		_environmentMapFaces[i]->init(GL_TEXTURE_2D, GL_RGBA, RenderSettings::EnvironmentWidth, RenderSettings::EnvironmentHeight, GL_RGBA, GL_FLOAT);

		//set texture filter options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//create framebuffer for the face and attach the texture and the renderbuffer
		_environmentFBOs.push_back(new Framebuffer());
		_environmentFBOs[i]->bind(GL_FRAMEBUFFER);
		_environmentFBOs[i]->attachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _environmentMapFaces[i]);
		_environmentFBOs[i]->attachRenderbuffer(GL_DEPTH_ATTACHMENT, _environmentRBO);

		//check for framebuffer completion
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "ERROR: Environment cubemap face framebuffer " + std::to_string(i) + " is incomplete." << std::endl;
		}
	}

	//create low quality cubemap of the scene environment
	_environmentMap = new Texture();
	_environmentMap->bind(GL_TEXTURE_CUBE_MAP);

	//init all cubemap faces
	for(unsigned int i = 0; i < 6; i++) {
		_environmentMap->init(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, GL_RGBA, RenderSettings::EnvironmentWidth, RenderSettings::EnvironmentHeight, GL_RGBA, GL_FLOAT);
	}

	//set texture filter options
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//create framebuffer to convert cube face into cubemap
	_environmentCubeFBO = new Framebuffer();
	_environmentCubeFBO->bind(GL_FRAMEBUFFER);

	//attach cubemap layers to framebuffer
	for(unsigned int i = 0; i < 6; i++) {
		_environmentCubeFBO->attachTexture(GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _environmentMap);
	}

	unsigned int attachments[6] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5};
	glDrawBuffers(6, attachments);

	//check for framebuffer completion
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: Environment cubemap framebuffer is incomplete." << std::endl;
	}

	//bind back to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::_initHdrFBO() {
	//create floating point color buffer
	_sceneColorBuffer = new Texture();
	_sceneColorBuffer->bind(GL_TEXTURE_2D);
	_sceneColorBuffer->init(GL_TEXTURE_2D, GL_RGBA16F, Window::ScreenWidth, Window::ScreenHeight, GL_RGBA, GL_FLOAT);

	//set texture filter options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//create floating point bright color buffer
	_brightColorBuffer = new Texture();
	_brightColorBuffer->bind(GL_TEXTURE_2D);
	_brightColorBuffer->init(GL_TEXTURE_2D, GL_RGBA16F, Window::ScreenWidth, Window::ScreenHeight, GL_RGBA, GL_FLOAT);

	//set texture filter options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//create depth renderbuffer
	_hdrRBO = new Renderbuffer();
	_hdrRBO->bind();
	_hdrRBO->init(GL_DEPTH_COMPONENT, Window::ScreenWidth, Window::ScreenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//create floating point framebuffer, attach color buffers and render buffer
	_hdrFBO = new Framebuffer();
	_hdrFBO->bind(GL_FRAMEBUFFER);
	_hdrFBO->attachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _sceneColorBuffer); //attach color buffer
	_hdrFBO->attachTexture(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _brightColorBuffer); //attach bright color buffer
	_hdrFBO->attachRenderbuffer(GL_DEPTH_ATTACHMENT, _hdrRBO); //attach depth renderbuffer

	//tell OpenGL which color attachments this framebuffer will use for rendering
	unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, attachments);

	//check for completion
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: HDR framebuffer is incomplete." << std::endl;
	}

	//bind back to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::_initBlurFBOs() {
	//create floating point blur colorbuffers
	for(unsigned int i = 0; i < 2; i++) {
		//create and bind framebuffer
		_bloomBlurFBOs[i] = new Framebuffer();
		_bloomBlurFBOs[i]->bind(GL_FRAMEBUFFER);

		//create texture buffers
		_blurColorBuffers[i] = new Texture();
		_blurColorBuffers[i]->bind(GL_TEXTURE_2D);
		_blurColorBuffers[i]->init(GL_TEXTURE_2D, GL_RGB16F, Window::ScreenWidth, Window::ScreenHeight, GL_RGB, GL_FLOAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //clamp to the edge as the blur filter would otherwise sample repeated texture values
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		_bloomBlurFBOs[i]->attachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _blurColorBuffers[i]); //attach blur colorbuffer to the blur framebuffer

		//check for framebuffer completion (no need for depth buffer)
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "ERROR: Bloom blur framebuffer " + std::to_string(i) + " is incomplete." << std::endl;
		}

		//bind back to default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void Renderer::_initSSAOFBOs() {
	//create ssao framebuffer
	_ssaoFBO = new Framebuffer();
	_ssaoFBO->bind(GL_FRAMEBUFFER);

	//create ssao color buffer
	_ssaoColorBuffer = new Texture();
	_ssaoColorBuffer->bind(GL_TEXTURE_2D);
	_ssaoColorBuffer->init(GL_TEXTURE_2D, GL_RED, Window::ScreenWidth, Window::ScreenHeight, GL_RGB, GL_FLOAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	_ssaoFBO->attachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ssaoColorBuffer); //attach buffer to fbo

	//check for completion
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: SSAO framebuffer is incomplete." << std::endl;
	}

	//create ssao blur framebuffer
	_ssaoBlurFBO = new Framebuffer();
	_ssaoBlurFBO->bind(GL_FRAMEBUFFER);

	//create ssao blur color buffer
	_ssaoBlurColorBuffer = new Texture();
	_ssaoBlurColorBuffer->bind(GL_TEXTURE_2D);
	_ssaoBlurColorBuffer->init(GL_TEXTURE_2D, GL_RED, Window::ScreenWidth, Window::ScreenHeight, GL_RGB, GL_FLOAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	_ssaoBlurFBO->attachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ssaoBlurColorBuffer); //attach buffer to fbo

	//check for completion
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: SSAO blur framebuffer is incomplete." << std::endl;
	}

	//bind back to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::_renderShadowMaps(std::vector<std::pair<RenderComponent*, glm::mat4>>& renderComponents, std::vector<glm::vec3>& pointLights, glm::mat4& lightSpaceMatrix) {
	//adjust viewport and bind to shadow framebuffer
	glViewport(0, 0, RenderSettings::ShadowWidth, RenderSettings::ShadowHeight);
	_shadowFBO->bind(GL_FRAMEBUFFER);
	glClear(GL_DEPTH_BUFFER_BIT);

	//setup shader uniforms
	_shadowShader->use();
	_shadowShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glm::mat4 modelMatrix;
	Material* material;
	Model* model;

	//render all models' depth into the shadow map from the lights perspective
	for(unsigned int i = 0; i < renderComponents.size(); i++) {
		modelMatrix = renderComponents[i].second;
		material = renderComponents[i].first->material;
		model = renderComponents[i].first->model;

		if(!material->getCastsShadows()) continue; //skip this model, if it should not cast shadows (e.g. like glass)

		_shadowShader->setMat4("modelMatrix", modelMatrix);
		model->draw();
	}

	//use shadow cubemap shader
	_shadowCubeShader->use();

	//render a cubemap for each point light
	std::vector<glm::mat4> shadowTransforms;
	glm::vec3 lightPos;

	for(unsigned int i = 0; i < pointLights.size(); i++) {
		//bind to correct cubemap shadow framebuffer
		_shadowCubeFBOs[i]->bind(GL_FRAMEBUFFER);
		glClear(GL_DEPTH_BUFFER_BIT);

		//create depth cubemap shadow matrices
		lightPos = pointLights[i];
		glm::mat4 shadowProjection = glm::perspective(glm::radians(90.0f), (float)RenderSettings::ShadowWidth / (float)RenderSettings::ShadowHeight, RenderSettings::CubeNearPlane, RenderSettings::CubeFarPlane);

		shadowTransforms.clear();
		shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

		//setup shader uniforms
		for(unsigned int i = 0; i < 6; i++) {
			_shadowCubeShader->setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
		}

		_shadowCubeShader->setFloat("farPlane", RenderSettings::CubeFarPlane);
		_shadowCubeShader->setVec3("lightPos", lightPos);

		//render all models' depth into the shadow cubemap from the lights perspective
		for(unsigned int i = 0; i < renderComponents.size(); i++) {
			modelMatrix = renderComponents[i].second;
			material = renderComponents[i].first->material;
			model = renderComponents[i].first->model;

			if(!material->getCastsShadows()) continue; //skip this model, if it should not cast shadows (e.g. like glass)

			_shadowCubeShader->setMat4("modelMatrix", modelMatrix);
			model->draw();
		}
	}

	//reset viewport
	glViewport(0, 0, Window::ScreenWidth, Window::ScreenHeight);
}

void Renderer::_renderEnvironmentMap(std::vector<std::pair<RenderComponent*, glm::mat4>>& renderComponents, Texture* skybox, glm::vec3& cameraPos, unsigned int pointLightCount) {
	//create depth cubemap shadow matrices
	glm::vec3 renderPos = _getClosestReflection(renderComponents, cameraPos);
	glm::mat4 environmentProjection = glm::perspective(glm::radians(90.0f), (float)RenderSettings::EnvironmentWidth / (float)RenderSettings::EnvironmentHeight, RenderSettings::CubeNearPlane, RenderSettings::CubeFarPlane);
	std::vector<glm::mat4> environmentViews;

	//view matrices for each cubemap face
	environmentViews.push_back(glm::lookAt(renderPos, renderPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f))); //right
	environmentViews.push_back(glm::lookAt(renderPos, renderPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f))); //left
	environmentViews.push_back(glm::lookAt(renderPos, renderPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f))); //top
	environmentViews.push_back(glm::lookAt(renderPos, renderPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f))); //bottom
	environmentViews.push_back(glm::lookAt(renderPos, renderPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f))); //front
	environmentViews.push_back(glm::lookAt(renderPos, renderPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))); //back

	//use shader and set viewport
	glViewport(0, 0, RenderSettings::EnvironmentWidth, RenderSettings::EnvironmentHeight);

	//forward render the scene objects without any special effects for each face
	glm::mat4 modelMatrix;
	Material* material;
	Model* model;

	for(unsigned int i = 0; i < 6; i++) {
		//bind framebuffer for the face
		_environmentFBOs[i]->bind(GL_FRAMEBUFFER);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//setup shader matrix
		_environmentShader->use();
		_environmentShader->setVec3("renderPos", renderPos);

		_environmentShader->setMat4("viewMatrixCube", environmentViews[i]);
		_environmentShader->setMat4("projectionMatrixCube", environmentProjection);

		//bind shadow map
		glActiveTexture(GL_TEXTURE1);
		_shadowMap->bind(GL_TEXTURE_2D);

		//render each scene object
		for(unsigned int i = 0; i < renderComponents.size(); i++) {
			modelMatrix = renderComponents[i].second;
			material = renderComponents[i].first->material;
			model = renderComponents[i].first->model;

			_environmentShader->setMat4("modelMatrix", modelMatrix);

			material->drawSimple(_environmentShader);
			model->draw();
		}

		//render skybox
		_renderSkybox(environmentViews[i], environmentProjection, skybox);
	}

	//render cubemap faces into cubemap using MRT
	_environmentCubeFBO->bind(GL_FRAMEBUFFER);
	glClear(GL_COLOR_BUFFER_BIT);

	//bind textures
	_faceToCubeShader->use();

	for(unsigned int i = 0; i < 6; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		_environmentMapFaces[i]->bind(GL_TEXTURE_2D);
	}

	//render screen quad with MRT
	_screenQuadVAO->bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	//reset viewport
	glViewport(0, 0, Window::ScreenWidth, Window::ScreenHeight);
}

void Renderer::_renderGeometry(std::vector<std::pair<RenderComponent*, glm::mat4>>& solidRenderComponents) {
	//bind to gBuffer framebuffer and render to buffer textures
	_gBuffer->bind(GL_FRAMEBUFFER);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 modelMatrix;
	Material* material;
	Model* model;

	for(unsigned int i = 0; i < solidRenderComponents.size(); i++) {
		modelMatrix = solidRenderComponents[i].second;
		material = solidRenderComponents[i].first->material;
		model = solidRenderComponents[i].first->model;

		material->drawDeferred(modelMatrix); //deferred
		model->draw();
	}
}

void Renderer::_renderSSAO() {
	//bind to ssao framebuffer
	_ssaoFBO->bind(GL_FRAMEBUFFER);
	glClear(GL_COLOR_BUFFER_BIT);

	//set ssao properties
	_ssaoShader->use();

	for(unsigned int i = 0; i < RenderSettings::SsaoKernelSize; i++) {
		_ssaoShader->setVec3("samples[" + std::to_string(i) + "]", _ssaoKernel[i]);
	}

	_ssaoShader->setFloat("screenWidth", (float)Window::ScreenWidth);
	_ssaoShader->setFloat("screenHeight", (float)Window::ScreenHeight);

	_ssaoShader->setInt("kernelSize", RenderSettings::SsaoKernelSize);
	_ssaoShader->setFloat("radius", RenderSettings::SsaoRadius);
	_ssaoShader->setFloat("bias", RenderSettings::SsaoBias);
	_ssaoShader->setFloat("power", RenderSettings::SsaoPower);

	//bind position color buffer
	glActiveTexture(GL_TEXTURE0);
	_gPositionRefract->bind(GL_TEXTURE_2D);

	//bind normal color buffer
	glActiveTexture(GL_TEXTURE1);
	_gNormalReflect->bind(GL_TEXTURE_2D);

	//bind noise texture
	glActiveTexture(GL_TEXTURE2);
	_ssaoNoiseTexture->bind(GL_TEXTURE_2D);

	//render quad
	_screenQuadVAO->bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Renderer::_renderSSAOBlur() {
	//bind to ssao blur framebuffer
	_ssaoBlurFBO->bind(GL_FRAMEBUFFER);
	glClear(GL_COLOR_BUFFER_BIT);

	//bind ssao input texture
	_ssaoBlurShader->use();

	glActiveTexture(GL_TEXTURE0);
	_ssaoColorBuffer->bind(GL_TEXTURE_2D);

	//render quad
	_screenQuadVAO->bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Renderer::_renderLighting(Texture* skybox, unsigned int pointLightCount) {
	//bind to hdr framebuffer
	_hdrFBO->bind(GL_FRAMEBUFFER);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//use lighting shader and bind textures
	_lightingShader->use();

	_lightingShader->setBool("useSSAO", RenderSettings::IsEnabled(RenderSettings::SSAO));

	//bind position color buffer
	glActiveTexture(GL_TEXTURE0);
	_gPositionRefract->bind(GL_TEXTURE_2D);

	//bind normal color buffer
	glActiveTexture(GL_TEXTURE1);
	_gNormalReflect->bind(GL_TEXTURE_2D);

	//bind albedo and specular color buffer
	glActiveTexture(GL_TEXTURE2);
	_gAlbedoSpec->bind(GL_TEXTURE_2D);

	//bind emission and shininess color buffer
	glActiveTexture(GL_TEXTURE3);
	_gEmissionShiny->bind(GL_TEXTURE_2D);

	//bind ssao texture
	glActiveTexture(GL_TEXTURE4);
	_ssaoBlurColorBuffer->bind(GL_TEXTURE_2D);

	//bind environment map
	glActiveTexture(GL_TEXTURE7);
	if(RenderSettings::IsEnabled(RenderSettings::EnvironmentMapping)) _environmentMap->bind(GL_TEXTURE_CUBE_MAP);
	else skybox->bind(GL_TEXTURE_CUBE_MAP);

	//bind shadow map
	glActiveTexture(GL_TEXTURE8);
	_shadowMap->bind(GL_TEXTURE_2D);

	//bind shadow cubemap
	for(unsigned int i = 0; i < pointLightCount; i++) {
		glActiveTexture(GL_TEXTURE9 + i);
		_shadowCubeMaps[i]->bind(GL_TEXTURE_CUBE_MAP);
	}

	//render quad
	_screenQuadVAO->bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Renderer::_renderScene(std::vector<std::pair<RenderComponent*, glm::mat4>>& renderComponents, Texture* skybox, unsigned int pointLightCount, bool useShadows, bool bindFBO) {
	//bind to hdr framebuffer if needed and render each renderable 
	if(bindFBO) {
		_hdrFBO->bind(GL_FRAMEBUFFER);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	//bind environment cubemap
	glActiveTexture(GL_TEXTURE7);
	if(RenderSettings::IsEnabled(RenderSettings::EnvironmentMapping)) _environmentMap->bind(GL_TEXTURE_CUBE_MAP);
	else skybox->bind(GL_TEXTURE_CUBE_MAP);

	//bind shadow maps
	if(useShadows) {
		glActiveTexture(GL_TEXTURE8);
		_shadowMap->bind(GL_TEXTURE_2D);

		for(unsigned int i = 0; i < pointLightCount; i++) {
			glActiveTexture(GL_TEXTURE9 + i);
			_shadowCubeMaps[i]->bind(GL_TEXTURE_CUBE_MAP);
		}
	}

	glm::mat4 modelMatrix;
	Material* material;
	Model* model;

	for(unsigned int i = 0; i < renderComponents.size(); i++) {
		modelMatrix = renderComponents[i].second;
		material = renderComponents[i].first->material;
		model = renderComponents[i].first->model;

		material->drawForward(modelMatrix); //forward
		model->draw();
	}
}

void Renderer::_renderSkybox(glm::mat4& viewMatrix, glm::mat4& projectionMatrix, Texture* skybox) {
	if(skybox == nullptr) {
		std::cout << "WARNING: Unable to render skybox, there is no skybox assigned." << std::endl;
		return;
	}

	//remove translation from view matrix
	glm::mat4 newViewMatrix = glm::mat4(glm::mat3(viewMatrix));

	//set shader uniforms
	_skyboxShader->use();
	_skyboxShader->setMat4("viewMatrix", newViewMatrix);
	_skyboxShader->setMat4("projectionMatrix", projectionMatrix);

	//render skybox (done in the same framebuffer as the one used in _renderScene)
	glDepthFunc(GL_LEQUAL); //set depth funtion to less than AND equal for skybox depth trick
	_skyboxVAO->bind();

	glActiveTexture(GL_TEXTURE0);
	skybox->bind(GL_TEXTURE_CUBE_MAP);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); //depth function set back to default
}

void Renderer::_renderPostProcessingQuad() {
	//blur bright fragments with two-pass Gaussian Blur 
	bool horizontal = true;
	bool firstIteration = true;

	_bloomBlurShader->use();

	//we will just use the quad and the first active texture, so we can just bind both once
	_screenQuadVAO->bind();
	glActiveTexture(GL_TEXTURE0);

	for(unsigned int i = 0; i < RenderSettings::BloomBlurAmount; i++) {
		_bloomBlurFBOs[horizontal]->bind(GL_FRAMEBUFFER);

		_bloomBlurShader->setInt("horizontal", horizontal);

		//bind texture of other framebuffer (or scene if first iteration)
		if(firstIteration) _brightColorBuffer->bind(GL_TEXTURE_2D);
		else _blurColorBuffers[!horizontal]->bind(GL_TEXTURE_2D);

		//render quad
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		horizontal = !horizontal;

		if(firstIteration) firstIteration = false;
	}

	//bind back to default framebuffer and render screen quad with the post processing effects
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set uniforms for gamma correction and tone mapping
	_postProcessingShader->use();
	_postProcessingShader->setBool("useBloom", RenderSettings::IsEnabled(RenderSettings::Bloom));
	_postProcessingShader->setBool("useFXAA", RenderSettings::IsEnabled(RenderSettings::FXAA));

	_postProcessingShader->setFloat("gamma", RenderSettings::Gamma);
	_postProcessingShader->setFloat("exposure", RenderSettings::Exposure);

	//set uniforms for FXAA
	_postProcessingShader->setVec2("inverseScreenTextureSize", glm::vec2(1.0f / (float)Window::ScreenWidth, 1.0f / (float)Window::ScreenHeight));
	_postProcessingShader->setFloat("fxaaSpanMax", RenderSettings::FxaaSpanMax);
	_postProcessingShader->setFloat("fxaaReduceMin", RenderSettings::FxaaReduceMin);
	_postProcessingShader->setFloat("fxaaReduceMul", RenderSettings::FxaaReduceMul);

	//bind texture
	glActiveTexture(GL_TEXTURE0);
	_sceneColorBuffer->bind(GL_TEXTURE_2D); //bind screen texture

	glActiveTexture(GL_TEXTURE1);
	_blurColorBuffers[!horizontal]->bind(GL_TEXTURE_2D); //bind blurred bloom texture

	//render texture to the screen and tone map and gamma correct
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Renderer::_getSortedRenderComponents(std::vector<Node*>& renderables, glm::vec3& cameraPos, std::vector<std::pair<RenderComponent*, glm::mat4>>& solidRenderables, std::vector<std::pair<RenderComponent*, glm::mat4>>& blendRenderables) {
	//fill both vectors with the objects
	std::vector<std::pair<RenderComponent*, glm::mat4>> newBlendRenderables;
	RenderComponent* renderComponent;
	Material* material;

	for(unsigned int i = 0; i < renderables.size(); i++) {
		renderComponent = (RenderComponent*)renderables[i]->getComponent(ComponentType::Render);
		material = renderComponent->material;

		std::pair<RenderComponent*, glm::mat4> renderPair;
		renderPair.first = renderComponent;
		renderPair.second = renderComponent->getOwner()->getTransform()->worldTransform;

		if(material->getBlendMode() == BlendMode::Opaque) {
			solidRenderables.push_back(renderPair);
		} else {
			newBlendRenderables.push_back(renderPair);
		}
	}

	//sort the blend objects based on the camera position
	std::map<float, unsigned int> sortedBlendObjects; //using a map, since it automatically sorts its entires by key (small first, ascending)
	glm::vec3 objectPos;
	float distance;

	for(unsigned int i = 0; i < newBlendRenderables.size(); i++) {
		objectPos = newBlendRenderables[i].first->getOwner()->getTransform()->getWorldPosition();

		distance = glm::distance2(cameraPos, objectPos); //squared distance, since we are only comparing distances against each other

		while(sortedBlendObjects.count(distance)) { //offset the distance slightly if there is an objects with the exact same distance already to allow having both in the map
			distance += 0.0001f;
		}

		sortedBlendObjects[distance] = i; //add entry to map
	}

	//add sorted pairs back to the blend vector by iterating backwards through the sorted map
	blendRenderables.clear();

	for(std::map<float, unsigned int>::reverse_iterator it = sortedBlendObjects.rbegin(); it != sortedBlendObjects.rend(); ++it) {
		blendRenderables.push_back(newBlendRenderables[it->second]);
	}
}

std::vector<glm::vec3> Renderer::_getClosestPointLights(glm::vec3 cameraPos, std::vector<std::pair<LightComponent*, glm::vec3>>& lightComponents) {
	//insert all point lights into the map to sort them automatically
	std::map<float, unsigned int> sortedPointLights;
	float distance;

	for(unsigned int i = 0; i < lightComponents.size(); i++) {
		if(lightComponents[i].first->lightType != LightType::Point) continue; //skip every non point light

		distance = glm::distance2(cameraPos, lightComponents[i].second); //distance squared for comparisons

		while(sortedPointLights.count(distance)) { //offset the distance slightly if there is an objects with the exact same distance already to allow having both in the map
			distance += 0.0001f;
		}

		sortedPointLights[distance] = i; //add index of light pair to map
	}

	//fill vector with the light entires closest to the camera
	unsigned int counter = 0;
	std::vector<glm::vec3> closestPointLights;

	for(std::map<float, unsigned int>::iterator it = sortedPointLights.begin(); it != sortedPointLights.end(); it++) {
		if(counter >= RenderSettings::MaxCubeShadows) break; //do not allow more point lights than specified in the settings

		closestPointLights.push_back(lightComponents[it->second].second); //add to new vector by indexing the unsorted one
		counter++;
	}

	return closestPointLights;
}

glm::vec3 Renderer::_getClosestReflection(std::vector<std::pair<RenderComponent*, glm::mat4>>& renderComponents, glm::vec3& cameraPos) {
	//fill map based on distance to camera (using only render components with reflection maps)
	std::map<float, glm::vec3> sortedReflectionObjects;
	glm::vec3 closestReflectionPos = glm::vec3(0.0f);

	RenderComponent* renderComponent;
	Material* material;

	glm::vec3 objectPos;
	TextureMaterial* textureMaterial;

	for(unsigned int i = 0; i < renderComponents.size(); i++) {
		renderComponent = renderComponents[i].first;
		material = renderComponent->material;

		if(material->getMaterialType() != MaterialType::Textures) continue; //only check texture materials

		objectPos = renderComponents[i].second[3];
		textureMaterial = (TextureMaterial*)material;

		//calcualte distance to camera and add map entry
		if(textureMaterial->getReflectionMap() != nullptr) {
			float distance = glm::distance2(cameraPos, objectPos);

			while(sortedReflectionObjects.count(distance)) {
				distance += 0.0001f;
			}

			sortedReflectionObjects[distance] = objectPos;
		}
	}

	//get the first value in the map by simply using its begin iterator
	if(sortedReflectionObjects.size() > 0) closestReflectionPos = sortedReflectionObjects.begin()->second; 

	return closestReflectionPos;
}

void Renderer::_fillUniformBuffers(glm::mat4& viewMatrix, glm::mat4& projectionMatrix, glm::mat4& lightSpaceMatrix, glm::vec3& cameraPos, glm::vec3& directionalLightPos, bool useShadows, std::vector<glm::vec3>& pointLightPositions) {
	//store the matrices in the matrices uniform buffer
	_matricesUBO->bind(GL_UNIFORM_BUFFER);
	_matricesUBO->bufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMatrix)); //buffer view matrix
	_matricesUBO->bufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projectionMatrix)); //buffer projection matrix
	_matricesUBO->bufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(lightSpaceMatrix)); //buffer light space matrix

	//store the data in the data uniform buffer
	unsigned int pointLightCount = pointLightPositions.size();

	_dataUBO->bind(GL_UNIFORM_BUFFER);
	_dataUBO->bufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLint), &useShadows); //buffer use shadows bool
	_dataUBO->bufferSubData(GL_UNIFORM_BUFFER, 4, sizeof(GLint), &pointLightCount); //buffer point light amount
	_dataUBO->bufferSubData(GL_UNIFORM_BUFFER, 8, sizeof(GLfloat), &RenderSettings::CubeFarPlane); //buffer cube map far plane

	_dataUBO->bufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(cameraPos)); //buffer cameraPos
	_dataUBO->bufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::vec4) * 2, sizeof(glm::vec4), glm::value_ptr(directionalLightPos)); //buffer directional light pos

	for(unsigned int i = 0; i < pointLightCount; i++) {
		_dataUBO->bufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::vec4) * 3 + sizeof(glm::vec4) * i, sizeof(glm::vec4), glm::value_ptr(pointLightPositions[i])); //buffer point light positions
	}

	glBindBuffer(GL_UNIFORM_BUFFER, 0); //unbind
}

void Renderer::_fillShaderStorageBuffers(std::vector<std::pair<LightComponent*, glm::vec3>>& lightComponents) {
	//store the lights in the lights shader storage buffer
	std::pair<LightComponent*, glm::vec3> currentLightPair;
	LightComponent* currentLight;
	glm::vec3 currentLightPos;

	unsigned int usedLights = lightComponents.size();
	if(usedLights > RenderSettings::MaxLights) usedLights = RenderSettings::MaxLights; //limit the amount of possible lights

	_lightsSSBO->bind(GL_SHADER_STORAGE_BUFFER);

	_dataUBO->bufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLint), &usedLights); //buffer used light amount

	for(unsigned int i = 0; i < usedLights; i++) {
		currentLight = lightComponents[i].first;
		currentLightPos = lightComponents[i].second;

		GLLight light = currentLight->toGLLight(currentLightPos); //convert current light component to a light struct GLSL can understand

		_dataUBO->bufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) + sizeof(GLLight) * i, sizeof(GLLight), &light); //buffer light struct (padded to the size of a vec4)
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); //unbind
}

void Renderer::_generateSSAOKernel() {
	//generate kernel samples in tangent space

	std::uniform_real_distribution<GLfloat> randomFloats(0.0f, 1.0f); //generates random floats between 0.0f and 1.0f
	std::default_random_engine generator;

	for(unsigned int i = 0; i < RenderSettings::SsaoKernelSize; i++) {
		glm::vec3 sample(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator));
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / (float)RenderSettings::SsaoKernelSize;

		//scale samples so that they're more aligned to center of kernel
		scale = Math::Lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;

		_ssaoKernel.push_back(sample);
	}
}

void Renderer::_generateNoiseTexture() {
	//generate random vectors
	std::uniform_real_distribution<GLfloat> randomFloats(0.0f, 1.0f);
	std::default_random_engine generator;

	std::vector<glm::vec3> ssaoNoise;

	for(unsigned int i = 0; i < 16; i++) {
		glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
		ssaoNoise.push_back(noise);
	}

	//generate small 4x4 noise texture
	_ssaoNoiseTexture = new Texture();
	_ssaoNoiseTexture->bind(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Renderer::_blitGDepthToHDR() {
	//blit the depth buffer of the gBuffer into the depth buffer of the hdr fbo
	_gBuffer->bind(GL_READ_FRAMEBUFFER);
	_hdrFBO->bind(GL_DRAW_FRAMEBUFFER);

	glBlitFramebuffer(0, 0, Window::ScreenWidth, Window::ScreenHeight, 0, 0, Window::ScreenWidth, Window::ScreenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}