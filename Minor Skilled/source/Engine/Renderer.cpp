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
	//vertices          //uv
	-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
};

Renderer::Renderer(unsigned int msaaSamples): _msaaSamples(msaaSamples) {
	glEnable(GL_DEPTH_TEST); //enable the z-buffer
	glDepthFunc(GL_LESS); //set depth funtion to less

	//glEnable(GL_BLEND); //enable blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //set blending function

    //glEnable(GL_STENCIL_TEST); //enable the stencil buffer

	//glEnable(GL_CULL_FACE); //enable face culling
	//glCullFace(GL_BACK); //cull backfaces

	glEnable(GL_MULTISAMPLE); //enable MSAA (only works in forward rendering and on the default framebuffer)

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); //enable seamless cubemap sampling for lower mip map levels in the pre filter map

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
	_initMultisampledHdrFBO();

	//setup post processing
	_initBloomFBOs();
	_initSSAOFBOs();

	_generateSSAOKernel();
	_generateNoiseTexture();
}

Renderer::~Renderer() {
	//free resources
	delete _lightingShader;
	delete _shadowShader;
	delete _skyboxShader;
	delete _ssaoShader;
	delete _ssaoBlurShader;
	delete _bloomBlurShader;
	delete _postProcessingShader;

	delete _gPosition;
	delete _gNormal;
	delete _gAlbedoSpec;
	delete _gEmissionShiny;

	delete _shadowMap;
	delete _multiSampledColorBuffer;
	delete _multiSampledBrightColorBuffer;
	delete _bloomBrightColorBuffer;

	for(unsigned int i = 0; i < 2; i++) {
		delete _blurColorBuffers[i];
	}

	delete _ssaoNoiseTexture;
	delete _ssaoColorBuffer;
	delete _ssaoBlurColorBuffer;

	glDeleteVertexArrays(1, &_skyboxVAO);
	glDeleteBuffers(1, &_skyboxVBO);

	glDeleteVertexArrays(1, &_screenQuadVAO);
	glDeleteBuffers(1, &_screenQuadVBO);

	glDeleteBuffers(1, &_matricesUBO);
	glDeleteBuffers(1, &_dataUBO);

	glDeleteBuffers(1, &_lightsSSBO);

	glDeleteFramebuffers(1, &_gBuffer);
	glDeleteFramebuffers(1, &_shadowFBO);
	glDeleteFramebuffers(1, &_multisampledHdrFBO);
	glDeleteFramebuffers(1, &_bloomFBO);
	glDeleteFramebuffers(2, _bloomBlurFBOs);
	glDeleteFramebuffers(1, &_ssaoFBO);
	glDeleteFramebuffers(1, &_ssaoBlurFBO);

	glDeleteRenderbuffers(1, &_gRBO);
	glDeleteRenderbuffers(1, &_multisampledHdrRBO);
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
	glm::vec3 directionalLightPos;

	glm::mat4 lightProjection;
	glm::mat4 lightView;
	glm::mat4 lightSpaceMatrix;

	bool useShadows = false;

	if(directionalLight != nullptr) {
		LightComponent* directionalLightComponent = (LightComponent*)directionalLight->getComponent(ComponentType::Light);
		directionalLightPos = glm::normalize(directionalLightComponent->lightDirection) * -3.0f; //offset light position 3 units in the opposite direction of the light direction

		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
		lightView = glm::lookAt(directionalLightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		lightSpaceMatrix = lightProjection * lightView;
	} else {
		useShadows = false;

		std::cout << "WARNING: No directional light assigned. Unable to render shadowmap." << std::endl;
	}

	//store the matrices and the vectors in the uniform buffer
	_fillUniformBuffers(viewMatrix, projectionMatrix, lightSpaceMatrix, cameraPos, directionalLightPos, useShadows);
	_fillShaderStorageBuffers(lightComponents);

	//clear screen in light grey
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	//render shadow map
	_renderShadowMap(renderComponents, lightSpaceMatrix);

	//render scene
	bool deferred = true;

	if(deferred) {
		//render the geometry of the scene (deferred shading)
		_renderSceneGeometry(solidRenderComponents);

		//render ssao texture
		_renderSSAO();

		//blur ssao texture
		_renderSSAOBlur();

		//render lighting of the scene (deferred shading)
		_renderSceneLighting();

		//blit gBuffer depth buffer into the hdr framebuffer depth buffer to enable forward rendering into the deferred scene
		_blitGDepthToHDR();
	} else {
		//render and light the scene (forward shading)
		_renderScene(solidRenderComponents, true); //bind the hdr here and clear buffer bits
	}

	//render blend objects (forward shading)
	glEnable(GL_BLEND);
	_renderScene(blendRenderComponents, false); //do not bind the hbr here and clear buffer bits since the solid objects are needed in the fbo
	glDisable(GL_BLEND);
	
	//render skybox
	_renderSkybox(viewMatrix, projectionMatrix, skybox);

	//blit multisampled bright color texture into the bloom fbo to get a non-multisampled bright color texture
	_blitHDRtoBloomFBO();

	//render screen quad
	_renderPostProcessingQuad(2.2f, 1.0f);
}

void Renderer::_initShaders() {
	//initialize lighting pass shader
	_lightingShader = new Shader(Filepath::ShaderPath + "post processing shader/screenQuad.vs", Filepath::ShaderPath + "post processing shader/lightingPass.fs");

	_lightingShader->use();
	_lightingShader->setInt("gPosition", 0);
	_lightingShader->setInt("gNormal", 1);
	_lightingShader->setInt("gAlbedoSpec", 2);
	_lightingShader->setInt("gEmissionShiny", 3);
	_lightingShader->setInt("ssao", 4);
	_lightingShader->setInt("shadowMap", 8);

	_lightingShader->setUniformBlockBinding("matricesBlock", 0); //set uniform block "matrices" to binding point 0
	_lightingShader->setUniformBlockBinding("dataBlock", 1); //set uniform block "data" to binding point 1

	_lightingShader->setShaderStorageBlockBinding("lightsBlock", 2); //set shader storage block "lights" to binding point 2

	//initialize shadow shader
	_shadowShader = new Shader(Filepath::ShaderPath + "shadow shader/shadow.vs", Filepath::ShaderPath + "shadow shader/shadow.fs");

	//initialize skybox shader
	_skyboxShader = new Shader(Filepath::ShaderPath + "skybox shader/skybox.vs", Filepath::ShaderPath + "skybox shader/skybox.fs");

	_skyboxShader->use();
	_skyboxShader->setInt("skybox", 0);

	//initialize ssao shader
	_ssaoShader = new Shader(Filepath::ShaderPath + "post processing shader/screenQuad.vs", Filepath::ShaderPath + "post processing shader/ssao.fs");

	_ssaoShader->use();
	_ssaoShader->setInt("gPosition", 0);
	_ssaoShader->setInt("gNormal", 1);
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
	_postProcessingShader->setInt("multiSampledScreenTexture", 0);
	_postProcessingShader->setInt("bloomBlur", 1);
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
	glBufferData(GL_UNIFORM_BUFFER, neededMemory, NULL, GL_STREAM_DRAW); //allocate the memory, but don't fill it with data yet
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//define the range of the buffer that links to a uniform binding point (binding point = 0)
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, _matricesUBO, 0, neededMemory);

	//create data uniform buffer
	neededMemory = sizeof(glm::vec4) * 3; //2x vec3 and a bool, but needs the memory layout of a three vec4 because of padding

	glGenBuffers(1, &_dataUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, _dataUBO);
	glBufferData(GL_UNIFORM_BUFFER, neededMemory, NULL, GL_STREAM_DRAW);

	glBindBufferRange(GL_UNIFORM_BUFFER, 1, _dataUBO, 0, neededMemory); //bind to binding point 1

	//unbind
	glBindBuffer(GL_UNIFORM_BUFFER, 0); 
}

void Renderer::_initShaderStorageBuffers() {
	//create lights shader storage buffer (enables use of dynamic arrays within shaders)
	unsigned int neededMemory = sizeof(glm::vec4) + sizeof(GLLight) * LightComponent::LightAmount; //112 bytes per light struct + 16 for an additional int

	glGenBuffers(1, &_lightsSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _lightsSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, neededMemory, NULL, GL_STATIC_DRAW); //tell OpenGL that we will change the data every time we use it by specifying GL_STREAM_DRAW

	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, _lightsSSBO, 0, neededMemory); //bind to binding point 2

	//unbind
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::_initGBuffer() {
	//create the position color buffer
	_gPosition = new Texture();
	glGenTextures(1, &_gPosition->getID());
	glBindTexture(GL_TEXTURE_2D, _gPosition->getID());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, Window::ScreenWidth, Window::ScreenHeight, 0, GL_RGB, GL_FLOAT, NULL); //32-bit precision RGB texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //ensure we don't accidentally oversample
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//create the normal color buffer
	_gNormal = new Texture();
	glGenTextures(1, &_gNormal->getID());
	glBindTexture(GL_TEXTURE_2D, _gNormal->getID());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, Window::ScreenWidth, Window::ScreenHeight, 0, GL_RGB, GL_FLOAT, NULL); //32-bit precision RGB texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//create the color + specular color buffer
	_gAlbedoSpec = new Texture();
	glGenTextures(1, &_gAlbedoSpec->getID());
	glBindTexture(GL_TEXTURE_2D, _gAlbedoSpec->getID());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Window::ScreenWidth, Window::ScreenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); //8-bit precision RGBA texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//create the emission color buffer
	_gEmissionShiny = new Texture();
	glGenTextures(1, &_gEmissionShiny->getID());
	glBindTexture(GL_TEXTURE_2D, _gEmissionShiny->getID());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Window::ScreenWidth, Window::ScreenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); //8-bit precision RGBA texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//create depth renderbuffer
	glGenRenderbuffers(1, &_gRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, _gRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Window::ScreenWidth, Window::ScreenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//create the gBuffer framebuffer and attach its color buffers for the deferred shading geometry pass
	glGenFramebuffers(1, &_gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gPosition->getID(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _gNormal->getID(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _gAlbedoSpec->getID(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, _gEmissionShiny->getID(), 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _gRBO);

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

void Renderer::_initMultisampledHdrFBO() {
	//create multisampled floating point color buffer
	_multiSampledColorBuffer = new Texture();

	glGenTextures(1, &_multiSampledColorBuffer->getID());
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _multiSampledColorBuffer->getID()); //multi sampled texture
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, _msaaSamples, GL_RGBA16F, Window::ScreenWidth, Window::ScreenHeight, GL_TRUE); 

	//create floating point bright color buffer
	_multiSampledBrightColorBuffer = new Texture();

	glGenTextures(1, &_multiSampledBrightColorBuffer->getID());
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _multiSampledBrightColorBuffer->getID()); //multi sampled texture
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, _msaaSamples, GL_RGBA16F, Window::ScreenWidth, Window::ScreenHeight, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	//create depth renderbuffer
	glGenRenderbuffers(1, &_multisampledHdrRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, _multisampledHdrRBO);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, _msaaSamples, GL_DEPTH_COMPONENT, Window::ScreenWidth, Window::ScreenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//create floating point framebuffer, attach color buffers and render buffer
	glGenFramebuffers(1, &_multisampledHdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _multisampledHdrFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, _multiSampledColorBuffer->getID(), 0); //attach color buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, _multiSampledBrightColorBuffer->getID(), 0); //attach bright color buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _multisampledHdrRBO); //attach depth renderbuffer

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

void Renderer::_initBloomFBOs() {
	//create bloom bright color buffer (not multisampled)
	_bloomBrightColorBuffer = new Texture();

	glGenTextures(1, &_bloomBrightColorBuffer->getID());
	glBindTexture(GL_TEXTURE_2D, _bloomBrightColorBuffer->getID()); //multi sampled texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::ScreenWidth, Window::ScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  //clamp to the edge as the blur filter would otherwise sample repeated texture values
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//create framebuffer
	glGenFramebuffers(1, &_bloomFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _bloomFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _bloomBrightColorBuffer->getID(), 0); //attach the color buffer to attachment 1, since the bright color in the other FBO is stored there

	//check for completion (no depth buffer needed)
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: Bloom framebuffer is incomplete" << std::endl;
	}

	//create blur framebuffers
	glGenFramebuffers(2, _bloomBlurFBOs);

	//create floating point blur colorbuffers
	for(unsigned int i = 0; i < 2; i++) {
		//bind framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, _bloomBlurFBOs[i]);

		//create texture buffers
		Texture* blurColorbuffer = new Texture();
		_blurColorBuffers[i] = blurColorbuffer; //store texture in the array

		glGenTextures(1, &blurColorbuffer->getID());
		glBindTexture(GL_TEXTURE_2D, blurColorbuffer->getID());

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Window::ScreenWidth, Window::ScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //clamp to the edge as the blur filter would otherwise sample repeated texture values
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurColorbuffer->getID(), 0); //attach blur colorbuffer to the blur framebuffer

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
	glGenFramebuffers(1, &_ssaoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _ssaoFBO);

	//create ssao color buffer
	_ssaoColorBuffer = new Texture();
	glGenTextures(1, &_ssaoColorBuffer->getID());
	glBindTexture(GL_TEXTURE_2D, _ssaoColorBuffer->getID());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, Window::ScreenWidth, Window::ScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ssaoColorBuffer->getID(), 0); //attach buffer to fbo

	//check for completion
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: SSAO framebuffer is incomplete." << std::endl;
	}

	//create ssao blur framebuffer
	glGenFramebuffers(1, &_ssaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _ssaoBlurFBO);

	//create ssao blur color buffer
	_ssaoBlurColorBuffer = new Texture();
	glGenTextures(1, &_ssaoBlurColorBuffer->getID());
	glBindTexture(GL_TEXTURE_2D, _ssaoBlurColorBuffer->getID());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, Window::ScreenWidth, Window::ScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ssaoBlurColorBuffer->getID(), 0); //attach buffer to fbo

	//check for completion
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: SSAO blur framebuffer is incomplete." << std::endl;
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
	Material* material;
	Model* model;

	for(unsigned int i = 0; i < renderComponents.size(); i++) {
		modelMatrix = renderComponents[i].second;
		material = renderComponents[i].first->material;
		model = renderComponents[i].first->model;

		if(!material->getCastsShadows()) continue; //skip this model, if it should not cast shadows (e.g. like glass)

		_shadowShader->setMat4("modelMatrix", modelMatrix);
		model->draw();
	}

	//reset viewport
	glViewport(0, 0, Window::ScreenWidth, Window::ScreenHeight);
}

void Renderer::_renderSceneGeometry(std::vector<std::pair<RenderComponent*, glm::mat4>>& solidRenderComponents) {
	//bind to gBuffer framebuffer and render to buffer textures
	glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);
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
	glBindFramebuffer(GL_FRAMEBUFFER, _ssaoFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	//set ssao properties
	_ssaoShader->use();

	for(int i = 0; i < 64; i++) {
		_ssaoShader->setVec3("samples[" + std::to_string(i) + "]", _ssaoKernel[i]);
	}

	_ssaoShader->setFloat("screenWidth", Window::ScreenWidth);
	_ssaoShader->setFloat("screenHeight", Window::ScreenHeight);

	_ssaoShader->setInt("kernelSize", 64);
	_ssaoShader->setFloat("radius", 0.5f);
	_ssaoShader->setFloat("bias", 0.025f);
	_ssaoShader->setFloat("power", 5.0f);

	//bind position color buffer
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _gPosition->getID());

	//bind normal color buffer
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _gNormal->getID());

	//bind noise texture
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _ssaoNoiseTexture->getID());

	//render quad
	glBindVertexArray(_screenQuadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Renderer::_renderSSAOBlur() {
	//bind to ssao blur framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, _ssaoBlurFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	//bind ssao input texture
	_ssaoBlurShader->use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _ssaoColorBuffer->getID());

	//render quad
	glBindVertexArray(_screenQuadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Renderer::_renderSceneLighting() {
	//bind to multisampled hdr framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, _multisampledHdrFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//use lighting shader and bind textures
	_lightingShader->use();

	//bind position color buffer
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _gPosition->getID());

	//bind normal color buffer
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _gNormal->getID());

	//bind albedo and specular color buffer
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _gAlbedoSpec->getID());

	//bind emission and shininess color buffer
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, _gEmissionShiny->getID());

	//bind ssao texture
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, _ssaoBlurColorBuffer->getID());

	//bind shadow map
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, _shadowMap->getID());

	//render quad
	glBindVertexArray(_screenQuadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Renderer::_renderScene(std::vector<std::pair<RenderComponent*, glm::mat4>>& renderComponents, bool bindFBO) {
	//bind to hdr framebuffer if needed and render each renderable 
	if(bindFBO) {
		glBindFramebuffer(GL_FRAMEBUFFER, _multisampledHdrFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

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
	glBindVertexArray(_skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getID());
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); //depth function set back to default
}

void Renderer::_renderPostProcessingQuad(float gamma, float exposure) {
	//blur bright fragments with two-pass Gaussian Blur 
	bool horizontal = true;
	bool firstIteration = true;
	unsigned int amount = 10;

	_bloomBlurShader->use();

	//we will just use the quad and the first active texture, so we can just bind both once
	glBindVertexArray(_screenQuadVAO);
	glActiveTexture(GL_TEXTURE0);

	for(unsigned int i = 0; i < amount; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, _bloomBlurFBOs[horizontal]);

		_bloomBlurShader->setInt("horizontal", horizontal);
		glBindTexture(GL_TEXTURE_2D, firstIteration ? _bloomBrightColorBuffer->getID() : _blurColorBuffers[!horizontal]->getID());  //bind texture of other framebuffer (or scene if first iteration)

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
	_postProcessingShader->setFloat("gamma", gamma);
	_postProcessingShader->setFloat("exposure", exposure);

	//set uniform for MSAA
	_postProcessingShader->setInt("msaaSamples", _msaaSamples);
	_postProcessingShader->setInt("screenWidth", Window::ScreenWidth);
	_postProcessingShader->setInt("screenHeight", Window::ScreenHeight);
	
	//bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _multiSampledColorBuffer->getID()); //bind multisampled texture

	glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, _blurColorBuffers[!horizontal]->getID()); //bind blurred bloom texture
	glBindTexture(GL_TEXTURE_2D, _ssaoBlurColorBuffer->getID());

	//render texture to the screen and tone map and gamma correct
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Renderer::_getSortedRenderComponents(std::vector<Node*>& renderables, glm::vec3& cameraPos, std::vector<std::pair<RenderComponent*, glm::mat4>>& solidRenderables, std::vector<std::pair<RenderComponent*, glm::mat4>>& blendRenderables) {
	//returns a vector of pairs  of render components and their respective model matrix (sorted by Opaque -> distance to camera)

	//fill both vectors with the objects
	RenderComponent* renderComponent;
	Material* material;

	for(unsigned int i = 0; i < renderables.size(); i++) {
		renderComponent = (RenderComponent*)renderables[i]->getComponent(ComponentType::Render);
		material = renderComponent->material;

		std::pair<RenderComponent*, glm::mat4> renderPair;
		renderPair.first = renderComponent;
		renderPair.second = renderComponent->getOwner()->getTransform()->worldTransform;

		if(material->getBlendMode() == BlendMode::Transparent) {
			blendRenderables.push_back(renderPair);
		} else {
			solidRenderables.push_back(renderPair);
		}
	}

	//sort the blend objects based on the camera position
	std::map<float, std::pair<RenderComponent*, glm::mat4>> sortedBlendObjects; //using a map, since it automatically sorts its entires by key
	glm::vec3 objectPos;

	for(unsigned int i = 0; i < blendRenderables.size(); i++) {
		objectPos = blendRenderables[i].first->getOwner()->getTransform()->getWorldPosition();

		float distance = glm::distance2(cameraPos, objectPos); //squared distance, since we are only comparing distances against each other

		while(sortedBlendObjects[distance].first != nullptr) { //offset the distance slightly if there is an objects with the exact same distance already to allow having both in the map
			distance += 0.0001f;
		}

		sortedBlendObjects[distance] = blendRenderables[i]; //add entry
	}
}

void Renderer::_fillUniformBuffers(glm::mat4& viewMatrix, glm::mat4& projectionMatrix, glm::mat4& lightSpaceMatrix, glm::vec3& cameraPos, glm::vec3& directionalLightPos, bool& useShadows) {
	//store the matrices in the matrices uniform buffer
	glBindBuffer(GL_UNIFORM_BUFFER, _matricesUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMatrix)); //buffer view matrix
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projectionMatrix)); //buffer projection matrix
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(lightSpaceMatrix)); //buffer light space matrix

	//store the data in the data uniform buffer
	glBindBuffer(GL_UNIFORM_BUFFER, _dataUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLboolean), &useShadows); //buffer use shadows bool
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(cameraPos)); //buffer cameraPos
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::vec4) * 2, sizeof(glm::vec4), glm::value_ptr(directionalLightPos)); //buffer directional light pos

	glBindBuffer(GL_UNIFORM_BUFFER, 0); //unbind
}

void Renderer::_fillShaderStorageBuffers(std::vector<std::pair<LightComponent*, glm::vec3>>& lightComponents) {
	//store the lights in the lights shader storage buffer
	std::pair<LightComponent*, glm::vec3> currentLightPair;
	LightComponent* currentLight;
	glm::vec3 currentLightPos;

	unsigned int usedLights = lightComponents.size();
	if(usedLights > LightComponent::LightAmount) usedLights = LightComponent::LightAmount; //limit the amount of possible lights

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _lightsSSBO);

	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLint), &usedLights); //buffer used light amount

	for(unsigned int i = 0; i < usedLights; i++) {
		currentLight = lightComponents[i].first;
		currentLightPos = lightComponents[i].second;

		GLLight light = currentLight->toGLLight(currentLightPos); //convert current light component to a light struct GLSL can understand

		glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) + sizeof(GLLight) * i, sizeof(GLLight), &light); //buffer light struct (padded to the size of a vec4)
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); //unbind
}

void Renderer::_generateSSAOKernel() {
	//generate kernel samples in tangent space

	std::uniform_real_distribution<GLfloat> randomFloats(0.0f, 1.0f); //generates random floats between 0.0f and 1.0f
	std::default_random_engine generator;

	for(unsigned int i = 0; i < 64; i++) {
		glm::vec3 sample(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator));
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / 64.0f;

		//scale samples so that they're more aligned to center of kernel
		scale = _lerp(0.1f, 1.0f, scale * scale);
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

	glGenTextures(1, &_ssaoNoiseTexture->getID());
	glBindTexture(GL_TEXTURE_2D, _ssaoNoiseTexture->getID());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Renderer::_blitGDepthToHDR() {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _multisampledHdrFBO);

	glBlitFramebuffer(0, 0, Window::ScreenWidth, Window::ScreenHeight, 0, 0, Window::ScreenWidth, Window::ScreenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

void Renderer::_blitHDRtoBloomFBO() {
	//blit from the multisampled framebuffer we rendered to into the non-multisampled framebuffer, so that we don't have to render the entire scene again
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _multisampledHdrFBO);
	glReadBuffer(GL_COLOR_ATTACHMENT1); //read from color attachment 1 (the bright color multisample texture)
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _bloomFBO);
	glDrawBuffer(GL_COLOR_ATTACHMENT0); //draw to color attachment 0

	//the resulting texture is stored in _bloomBrightColorBuffer
	glBlitFramebuffer(0, 0, Window::ScreenWidth, Window::ScreenHeight, 0, 0, Window::ScreenWidth, Window::ScreenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

float Renderer::_lerp(float a, float b, float f) {
	return a + f * (b - a); //linear interpolation
}
