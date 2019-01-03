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
#include "../Engine/Debug.h"

#include "../Materials/TextureMaterial.h"

#include "../Components/LightComponent.h"
#include "../Components/CameraComponent.h"
#include "../Components/RenderComponent.h"

#include "../Utility/Filepath.h"
#include "../Utility/ComponentType.h"
#include "../Utility/Math.h"
#include "../Utility/RenderSettings.h"

const std::vector<float> Renderer::_SkyboxVertices = {
	// back face
	-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
	1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
	1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
	1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
	-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
	-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
	// front face
	-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
	1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
	1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
	1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
	-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
	-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
	// left face
	-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
	-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
	-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
	-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
	-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
	-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
	// right face
	1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
	1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
	1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
	1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
	1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
	1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
	// bottom face
	-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
	1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
	1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
	1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
	-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
	-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
	// top face
	-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
	1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
	1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
	1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
	-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
	-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left 
};

const std::vector<float> Renderer::_ScreenQuadVertices = {
	//vertices          //uv
	-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
};

Renderer::Renderer(Debug* profiler) : _profiler(profiler) {
	glEnable(GL_DEPTH_TEST); //enable the z-buffer
	glDepthFunc(GL_LESS); //set depth funtion to less

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //set blending function

    //glEnable(GL_STENCIL_TEST); //enable the stencil buffer

	//glEnable(GL_CULL_FACE); //enable face culling
	//glCullFace(GL_BACK); //cull backfaces

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); //enable seamless cubemap sampling for lower mip map levels in the pre filter map

	//glEnable(GL_MULTISAMPLE); //enable MSAA (only works in forward rendering and on the default framebuffer)

	if(RenderSettings::VSync) {
		glfwSwapInterval(1); //enable vsync
		_vSync = true;
	} else {
		glfwSwapInterval(0); //disable vysnc
		_vSync = false;
	}

	//shaders
	_initShaders();

	//setup VAOs and VBOs
	_initSkyboxVAO();
	_initScreenQuadVAO();

	//setup UBOs and SSBOs
	_initUniformBuffers();
	_initShaderStorageBuffers();

	//setup FBOs
	_initGBuffers();

	_initConversionFBO();
	_initShadowFBO();
	_initShadowCubeFBO();
	_initDepthFBO();
	_initEnvironmentFBO();
	_initHdrFBO();

	//setup post processing
	_initBlurFBOs();
	_initSSAOFBOs();
	_initSSRFBO();

	_generateSSAOKernel();
	_generateNoiseTexture();
}

Renderer::~Renderer() {
	//delete environment maps
	for(std::map<RenderComponent*, Texture*>::iterator it = _environmentMaps.begin(); it != _environmentMaps.end(); it++) {
		delete it->second;
	}

	for(std::map<RenderComponent*, IBLMaps>::iterator it = _iblMaps.begin(); it != _iblMaps.end(); it++) {
		delete it->second.environmentMap;
		delete it->second.irradianceMap;
		delete it->second.prefilterMap;
	}

	delete _brdfLUT;

	//delete shaders
	delete _equiToCubeShader;
	delete _lightingShader;
	delete _lightingShaderPbr;
	delete _shadowShader;
	delete _shadowCubeShader;
	delete _depthShader;
	delete _environmentShader;
	delete _irradianceShader;
	delete _prefilterShader;
	delete _brdfShader;
	delete _skyboxShader;
	delete _ssaoShader;
	delete _ssaoBlurShader;
	delete _ssrShader;
	delete _bloomBlurShader;
	delete _postProcessingShader;

	//delete textures
	delete _gPosition;
	delete _gNormal;
	delete _gAlbedo;
	delete _gEmissionSpec;

	delete _gEnvironmentShiny;

	delete _gMetalRoughAO;
	delete _gIrradiance;
	delete _gPrefilter;
	delete _gReflectance;

	delete _shadowMap;

	for(unsigned int i = 0; i < _shadowCubeMaps.size(); i++) {
		delete _shadowCubeMaps[i];
	}

	delete _sceneDepthBuffer;
	delete _sceneColorBuffer;
	delete _brightColorBuffer;
	delete _blurColorBuffers[1];
	delete _blurColorBuffers[0];

	delete _ssaoNoiseTexture;
	delete _ssaoColorBuffer;
	delete _ssaoBlurColorBuffer;

	delete _ssrColorBuffer;

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
	delete _gBufferPbr;

	delete _conversionFBO;
	delete _shadowFBO;
	delete _shadowCubeFBO;
	delete _depthFBO;

	delete _environmentFBO;
	delete _hdrFBO;
	delete _bloomFBO;
	delete _bloomBlurFBOs[0]; 
	delete _bloomBlurFBOs[1];
	delete _ssaoFBO;
	delete _ssaoBlurFBO;
	delete _ssrFBO;

	//delete renderbuffers
	delete _gRBO;

	delete _conversionRBO;
	delete _environmentRBO;
	delete _hdrRBO;
}

void Renderer::render(std::vector<Node*>& renderables, std::vector<Node*>& lights, Node* mainCamera, Node* directionalLight, Texture* skybox) {
	//update dimensions if needed
	_updateDimensions();
	_updateVSync();

	//render from main camera
	if(mainCamera == nullptr) {
		std::cout << "ERROR: There is no main camera assigned. Unable to render scene." << std::endl;
		return;
	}

	CameraComponent* mainCameraComponent = (CameraComponent*)mainCamera->getComponent(ComponentType::Camera);
	Transform* mainCameraTransform = mainCamera->getTransform();

	glm::mat4 projectionMatrix = mainCameraComponent->getProjectionMatrix();
	glm::mat4 viewMatrix = mainCameraComponent->getViewMatrix();
	glm::mat4 previousViewProjectionMatrix = mainCameraComponent->getPreviousViewProjectionMatrix();
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
	} else if(useShadows) {
		useShadows = false;

		std::cout << "WARNING: No directional light assigned. Unable to render shadowmap." << std::endl;
	}

	//get closest point lights
	if(useShadows) pointLightPositions = _getClosestPointLights(cameraPos, lightComponents);
	pointLightCount = pointLightPositions.size();

	//store the matrices and the vectors in the uniform buffer
	_fillUniformBuffers(viewMatrix, projectionMatrix, previousViewProjectionMatrix, lightSpaceMatrix, cameraPos, directionalLightPos, useShadows, pointLightPositions);
	_fillShaderStorageBuffers(lightComponents);

	//clear screen in light grey
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	_profiler->startQuery(QueryType::Rendering);

	//render shadow map
	if(useShadows) {
		_profiler->startQuery(QueryType::Shadow);
		_renderShadowMaps(renderComponents, pointLightPositions, lightSpaceMatrix);
		_profiler->endQuery(QueryType::Shadow);
	}

	//render the depth of the scene
	_profiler->startQuery(QueryType::Depth);
	_renderDepth(renderComponents);
	_profiler->endQuery(QueryType::Depth);

	//render scene
	bool pbr = RenderSettings::IsEnabled(RenderSettings::PBR);
	bool deferred = RenderSettings::IsEnabled(RenderSettings::Deferred);

	if(deferred) {
		//render the geometry of the scene (deferred shading)
		_profiler->startQuery(QueryType::Geometry);
		_renderGeometry(solidRenderComponents, pbr);
		_profiler->endQuery(QueryType::Geometry);

		if(RenderSettings::IsEnabled(RenderSettings::SSAO)) {
			_profiler->startQuery(QueryType::SSAO);

			//render ssao texture (deferred shading)
			_renderSSAO();

			//blur ssao texture (deferred shading)
			_renderSSAOBlur();

			_profiler->endQuery(QueryType::SSAO);
		}

		//render lighting of the scene (deferred shading)
		_profiler->startQuery(QueryType::Lighting);
		_renderLighting(skybox, pointLightCount, pbr);
		_profiler->endQuery(QueryType::Lighting);

		//blit gBuffer depth buffer into the hdr framebuffer depth buffer to enable forward rendering into the deferred scene
		_blitGDepthToHDR(pbr);
	} else {
		//render and light the scene (forward shading)
		_renderScene(solidRenderComponents, pointLightCount, useShadows, true); //bind the hdr here and clear buffer bits
	}

	//render skybox
	_renderSkybox(viewMatrix, projectionMatrix, skybox);

	//render blend objects (forward shading)
	glEnable(GL_BLEND);
	_profiler->startQuery(QueryType::Blending);
	_renderScene(blendRenderComponents, pointLightCount, useShadows, false); //do not bind the hbr here and clear buffer bits since the solid objects are needed in the fbo
	_profiler->endQuery(QueryType::Blending);
	glDisable(GL_BLEND);

	//render the screen space reflection texture after the scene was rendered to use the final frame (deferred shading)
	if(deferred && RenderSettings::IsEnabled(RenderSettings::SSR)) {
		_profiler->startQuery(QueryType::SSR);
		_renderSSR(mainCameraComponent);
		_profiler->endQuery(QueryType::SSR);
	}

	//render screen quad
	_profiler->startQuery(QueryType::PostProcessing);
	_renderPostProcessingQuad();
	_profiler->endQuery(QueryType::PostProcessing);
	_profiler->endQuery(QueryType::Rendering);
}

void Renderer::renderEnvironmentMaps(std::vector<Node*>& renderables, Node* directionalLight, Texture* skybox) {
	//obtain all render components and their model matrices from the renderables vector
	std::vector<std::pair<RenderComponent*, glm::mat4>> renderComponents;

	for(unsigned int i = 0; i < renderables.size(); i++) {
		std::pair<RenderComponent*, glm::mat4> renderPair;
		renderPair.first = (RenderComponent*)renderables[i]->getComponent(ComponentType::Render);
		renderPair.second = renderables[i]->getTransform()->worldTransform;

		renderComponents.push_back(renderPair); //add to vector
	}

	//obtain directional light information
	LightComponent* directionalLightComponent;

	if(directionalLight != nullptr && directionalLight->hasComponent(ComponentType::Light)) directionalLightComponent = (LightComponent*)directionalLight->getComponent(ComponentType::Light);
	else directionalLightComponent = nullptr;

	//render all environment maps for all texture materials with reflection map
	std::cout << "Rendering environment maps..." << std::endl;

	glm::mat4 environmentProjection = glm::perspective(glm::radians(90.0f), (float)RenderSettings::EnvironmentWidth / (float)RenderSettings::EnvironmentHeight, RenderSettings::CubeNearPlane, RenderSettings::CubeFarPlane);
	
	glm::vec3 renderPos;
	RenderComponent* renderComponent;
	TextureMaterial* textureMat;
	Texture* environmentMap;
	MaterialType materialType;

	//set viewport and bind to cubemap framebuffer
	glViewport(0, 0, RenderSettings::EnvironmentWidth, RenderSettings::EnvironmentHeight);
	_environmentFBO->bind(GL_FRAMEBUFFER);

	for(unsigned int i = 0; i < renderComponents.size(); i++) {
		//check for material types
		renderComponent = renderComponents[i].first;
		materialType = renderComponent->material->getMaterialType();

		if(materialType == MaterialType::Color) continue; //skip color materials

		if(materialType == MaterialType::Textures) {
			textureMat = (TextureMaterial*)renderComponent->material;

			if(textureMat->getReflectionMap() == nullptr) continue; //skip texture material without reflection maps
		}

		renderPos = renderComponents[i].second[3]; //world position component of the model matrix

		//render low quality environment map
		environmentMap = _renderEnvironmentMap(renderComponents, environmentProjection, renderPos, skybox, directionalLightComponent, materialType == MaterialType::PBR);

		if(materialType == MaterialType::Textures) {
			//add environment map to the map for texture materials
			_environmentMaps[renderComponent] = environmentMap; 
		} else {
			//add environment map to the map for pbr materials
			IBLMaps maps;
			maps.environmentMap = environmentMap;
			_iblMaps[renderComponent] = maps;
		}
	}

	//render all IBL maps for all pbr materials
	std::cout << "Rendering IBL maps..." << std::endl;

	environmentProjection = glm::perspective(glm::radians(90.0f), 1.0f, RenderSettings::CubeNearPlane, RenderSettings::CubeFarPlane);

	for(std::map<RenderComponent*, IBLMaps>::iterator it = _iblMaps.begin(); it != _iblMaps.end(); it++) {
		IBLMaps* maps = &it->second;
		renderPos = it->first->getOwner()->getTransform()->getWorldPosition();

		maps->irradianceMap = _renderIrradianceMap(maps->environmentMap, environmentProjection); //add irradiance map
		maps->prefilterMap = _renderPrefilterMap(maps->environmentMap, environmentProjection); //add prefilter map 
	}

	//render BRDF lookup texture
	_renderBrdfLUT();

	//reset viewport and bind back to default framebuffer
	glViewport(0, 0, Window::ScreenWidth, Window::ScreenHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Texture* Renderer::convertEquiToCube(Texture* skybox) {
	std::cout << "Converting equirectangular texture to cubemap..." << std::endl;

	//create new cubemap texture
	Texture* cubemap = new Texture();
	cubemap->bind(GL_TEXTURE_CUBE_MAP);

	//init each cubemap face
	for(unsigned int i = 0; i < 6; ++i) {
		cubemap->init(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, GL_RGB16F, RenderSettings::SkyboxWidth, RenderSettings::SkyboxHeight, GL_RGB, GL_FLOAT, NULL);
	}

	cubemap->filter(GL_TEXTURE_CUBE_MAP, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);

	//setup projection and view matrices
	glm::mat4 conversionProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 conversionViews[] = {
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	//convert HDR equirectangular environment map to cubemap equivalent
	_equiToCubeShader->use();
	_equiToCubeShader->setMat4("projectionMatrix", conversionProjection);

	//bind equirectangular input texture
	glActiveTexture(GL_TEXTURE0);
	skybox->bind(GL_TEXTURE_2D);

	//set viewport and bind to conversion framebuffer
	glViewport(0, 0, RenderSettings::SkyboxWidth, RenderSettings::SkyboxHeight);
	_conversionFBO->bind(GL_FRAMEBUFFER);
	_skyboxVAO->bind();

	for(unsigned int i = 0; i < 6; ++i) {
		//attach current face of the cubemap to the fbo
		_conversionFBO->attachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_equiToCubeShader->setMat4("viewMatrix", conversionViews[i]);

		//render the cube
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	//bind back to default framebuffer and reset viewport
	glViewport(0, 0, Window::ScreenWidth, Window::ScreenHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);

	delete skybox; //the input texture is not needed anymore

	//return new cubemap
	return cubemap;
}

void Renderer::_initShaders() {
	//initialize equirectangular to cubemap shader
	_equiToCubeShader = new Shader(Filepath::ShaderPath + "skybox shader/cube.vs", Filepath::ShaderPath + "skybox shader/equiToCube.fs");

	_equiToCubeShader->use();
	_equiToCubeShader->setInt("equirectangularMap", 0);

	//initialize lighting pass shader
	_lightingShader = new Shader(Filepath::ShaderPath + "post processing shader/screenQuad.vs", Filepath::ShaderPath + "post processing shader/lightingPass.fs");

	_lightingShader->use();
	_lightingShader->setInt("gPosition", 0);
	_lightingShader->setInt("gNormal", 1);
	_lightingShader->setInt("gAlbedo", 2);
	_lightingShader->setInt("gEmissionSpec", 3);
	_lightingShader->setInt("gEnvironmentShiny", 4);

	_lightingShader->setInt("ssao", 8);

	_lightingShader->setInt("shadowMap", 11);

	for(unsigned int i = 0; i < RenderSettings::MaxCubeShadows; i++) {
		_lightingShader->setInt("shadowCubemaps[" + std::to_string(i) + "]", 12 + i);
	}

	_lightingShader->setUniformBlockBinding("matricesBlock", 0); //set uniform block "matrices" to binding point 0
	_lightingShader->setUniformBlockBinding("dataBlock", 1); //set uniform block "data" to binding point 1

	_lightingShader->setShaderStorageBlockBinding("lightsBlock", 2); //set shader storage block "lights" to binding point 2

	//initialize lighting pass pbr shader
	_lightingShaderPbr = new Shader(Filepath::ShaderPath + "post processing shader/screenQuad.vs", Filepath::ShaderPath + "post processing shader/lightingPassPbr.fs");

	_lightingShaderPbr->use();
	_lightingShaderPbr->setInt("gPosition", 0);
	_lightingShaderPbr->setInt("gNormal", 1);
	_lightingShaderPbr->setInt("gAlbedo", 2);
	_lightingShaderPbr->setInt("gEmissionSpec", 3);
	_lightingShaderPbr->setInt("gMetalRoughAO ", 4);
	_lightingShaderPbr->setInt("gIrradiance ", 5);
	_lightingShaderPbr->setInt("gPrefilter", 6);
	_lightingShaderPbr->setInt("gReflectance", 7);

	_lightingShaderPbr->setInt("ssao", 8);
	_lightingShaderPbr->setInt("brdfLUT", 9);

	_lightingShaderPbr->setInt("shadowMap", 11);

	for(unsigned int i = 0; i < RenderSettings::MaxCubeShadows; i++) {
		_lightingShaderPbr->setInt("shadowCubemaps[" + std::to_string(i) + "]", 12 + i);
	}

	_lightingShaderPbr->setUniformBlockBinding("matricesBlock", 0); //set uniform block "matrices" to binding point 0
	_lightingShaderPbr->setUniformBlockBinding("dataBlock", 1); //set uniform block "data" to binding point 1

	_lightingShaderPbr->setShaderStorageBlockBinding("lightsBlock", 2); //set shader storage block "lights" to binding point 2

	//initialize shadow shader
	_shadowShader = new Shader(Filepath::ShaderPath + "depth shader/shadow.vs", Filepath::ShaderPath + "depth shader/shadow.fs");

	//initialize shadow cubemap shader (including geometry shader for layered rendering)
	_shadowCubeShader = new Shader(Filepath::ShaderPath + "depth shader/shadowCube.vs", Filepath::ShaderPath + "depth shader/shadowCube.gs", Filepath::ShaderPath + "depth shader/shadowCube.fs");

	//initialize scene depth shader
	_depthShader = new Shader(Filepath::ShaderPath + "depth shader/depth.vs", Filepath::ShaderPath + "depth shader/depth.fs");

	_depthShader->use();
	_depthShader->setUniformBlockBinding("matricesBlock", 0); //set uniform block "matrices" to binding point 0

	//initialize environment shader
	_environmentShader = new Shader(Filepath::ShaderPath + "skybox shader/environment.vs", Filepath::ShaderPath + "skybox shader/environment.fs");

	_environmentShader->use();
	_environmentShader->setInt("diffuseMap", 0);

	//initialize irradiance shader
	_irradianceShader = new Shader(Filepath::ShaderPath + "skybox shader/cube.vs", Filepath::ShaderPath + "skybox shader/irradiance.fs");

	_irradianceShader->use();
	_irradianceShader->setInt("environmentMap", 0);

	//initialize prefilter shader
	_prefilterShader = new Shader(Filepath::ShaderPath + "skybox shader/cube.vs", Filepath::ShaderPath + "skybox shader/prefilter.fs");

	_prefilterShader->use();
	_prefilterShader->setInt("environmentMap", 0);

	//initialize brdf shader
	_brdfShader = new Shader(Filepath::ShaderPath + "skybox shader/brdf.vs", Filepath::ShaderPath + "skybox shader/brdf.fs");

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

	//initialize ssr shader
	_ssrShader = new Shader(Filepath::ShaderPath + "post processing shader/screenQuad.vs", Filepath::ShaderPath + "post processing shader/ssr.fs");

	_ssrShader->use();
	_ssrShader->setInt("gNormal", 0);
	_ssrShader->setInt("gEmissionSpec", 1);
	_ssrShader->setInt("sceneTexture", 2);
	_ssrShader->setInt("depthTexture", 3);

	_ssrShader->setUniformBlockBinding("matricesBlock", 0); //set uniform block "matrices" to binding point 0

	//initialize bloom blur shader
	_bloomBlurShader = new Shader(Filepath::ShaderPath + "post processing shader/screenQuad.vs", Filepath::ShaderPath + "post processing shader/bloomBlur.fs");

	_bloomBlurShader->use();
	_bloomBlurShader->setInt("image", 0);

	//initialize post processing shader
	_postProcessingShader = new Shader(Filepath::ShaderPath + "post processing shader/screenQuad.vs", Filepath::ShaderPath + "post processing shader/postProcessing.fs");

	_postProcessingShader->use();
	_postProcessingShader->setInt("sceneTexture", 0);
	_postProcessingShader->setInt("depthTexture", 1);
	_postProcessingShader->setInt("bloomBlur", 2);
	_postProcessingShader->setInt("ssr", 3);
}

void Renderer::_initSkyboxVAO() {
	//setup skybox VAO and VBO
	_skyboxVAO = new VertexArray();
	_skyboxVAO->bind();

	_skyboxVBO = new Buffer();
	_skyboxVBO->bind(GL_ARRAY_BUFFER);
	_skyboxVBO->bufferData(GL_ARRAY_BUFFER, &_SkyboxVertices[0], _SkyboxVertices.size() * sizeof(float));

	glEnableVertexAttribArray(0); //vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1); //normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2); //uv
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
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
	unsigned int neededMemory = sizeof(glm::mat4) * 4; //4x mat4

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

void Renderer::_initGBuffers() {
	//init the color buffers for both gBuffers

	//create the position color buffer
	_gPosition = new Texture();
	_gPosition->bind(GL_TEXTURE_2D);
	_gPosition->init(GL_TEXTURE_2D, GL_RGB16F, Window::ScreenWidth, Window::ScreenHeight, GL_RGB, GL_FLOAT, NULL); //16-bit precision RGB texture
	_gPosition->filter(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);

	//create the normal color buffer
	_gNormal = new Texture();
	_gNormal->bind(GL_TEXTURE_2D);
	_gNormal->init(GL_TEXTURE_2D, GL_RGB16F, Window::ScreenWidth, Window::ScreenHeight, GL_RGB, GL_FLOAT, NULL); //16-bit precision RGB texture
	_gNormal->filter(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);

	//create the albedo color buffer
	_gAlbedo = new Texture();
	_gAlbedo->bind(GL_TEXTURE_2D);
	_gAlbedo->init(GL_TEXTURE_2D, GL_RGB16F, Window::ScreenWidth, Window::ScreenHeight, GL_RGB, GL_FLOAT, NULL); //16-bit precision RGB texture
	_gAlbedo->filter(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR, GL_NONE);

	//create the emission + specular color buffer
	_gEmissionSpec = new Texture();
	_gEmissionSpec->bind(GL_TEXTURE_2D);
	_gEmissionSpec->init(GL_TEXTURE_2D, GL_RG16F, Window::ScreenWidth, Window::ScreenHeight, GL_RG, GL_FLOAT, NULL); //16-bit precision RG texture
	_gEmissionSpec->filter(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR, GL_NONE);

	//init the color buffers for the normal gBuffer

	//create the environment + shininess color buffer
	_gEnvironmentShiny = new Texture();
	_gEnvironmentShiny->bind(GL_TEXTURE_2D);
	_gEnvironmentShiny->init(GL_TEXTURE_2D, GL_RGBA16F, Window::ScreenWidth, Window::ScreenHeight, GL_RGBA, GL_FLOAT, NULL); //16-bit precision RGBA texture
	_gEnvironmentShiny->filter(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR, GL_NONE);

	//init the color buffers for the PBR gBuffer

	//create metallic + roughness + ambient occlusion color buffer
	_gMetalRoughAO = new Texture();
	_gMetalRoughAO->bind(GL_TEXTURE_2D);
	_gMetalRoughAO->init(GL_TEXTURE_2D, GL_RGB16F, Window::ScreenWidth, Window::ScreenHeight, GL_RGB, GL_FLOAT, NULL); //16-bit precision RGB texture
	_gMetalRoughAO->filter(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR, GL_NONE);

	//create the irradiance color buffer
	_gIrradiance = new Texture();
	_gIrradiance->bind(GL_TEXTURE_2D);
	_gIrradiance->init(GL_TEXTURE_2D, GL_RGB16F, Window::ScreenWidth, Window::ScreenHeight, GL_RGB, GL_FLOAT, NULL); //16-bit precision RGB texture
	_gIrradiance->filter(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR, GL_NONE);

	//create the prefilter color buffer
	_gPrefilter = new Texture();
	_gPrefilter->bind(GL_TEXTURE_2D);
	_gPrefilter->init(GL_TEXTURE_2D, GL_RGB16F, Window::ScreenWidth, Window::ScreenHeight, GL_RGB, GL_FLOAT, NULL); //16-bit precision RGB texture
	_gPrefilter->filter(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR, GL_NONE);

	//create the reflectance (F0) color buffer
	_gReflectance = new Texture();
	_gReflectance->bind(GL_TEXTURE_2D);
	_gReflectance->init(GL_TEXTURE_2D, GL_RGB16F, Window::ScreenWidth, Window::ScreenHeight, GL_RGB, GL_FLOAT, NULL); //16-bit precision RGB texture
	_gReflectance->filter(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR, GL_NONE);

	//create depth renderbuffer
	_gRBO = new Renderbuffer();
	_gRBO->bind();
	_gRBO->init(GL_DEPTH_COMPONENT, Window::ScreenWidth, Window::ScreenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//create the gBuffer framebuffer and attach its color buffers for the deferred shading geometry pass
	_gBuffer = new Framebuffer();
	_gBuffer->bind(GL_FRAMEBUFFER);
	_gBuffer->attachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gPosition);
	_gBuffer->attachTexture(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _gNormal);
	_gBuffer->attachTexture(GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _gAlbedo);
	_gBuffer->attachTexture(GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, _gEmissionSpec);
	_gBuffer->attachTexture(GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, _gEnvironmentShiny);
	_gBuffer->attachRenderbuffer(GL_DEPTH_ATTACHMENT, _gRBO);

	//tell OpenGL which attachments the gBuffer will use for rendering
	unsigned int attachments[5] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4};
	glDrawBuffers(5, attachments);

	//check for completion
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: gBuffer framebuffer is incomplete." << std::endl;
	}

	//create the gBuffer framebuffer and attach its color buffers for the deferred shading geometry pass
	_gBufferPbr = new Framebuffer();
	_gBufferPbr->bind(GL_FRAMEBUFFER);
	_gBufferPbr->attachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gPosition);
	_gBufferPbr->attachTexture(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _gNormal);
	_gBufferPbr->attachTexture(GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _gAlbedo);
	_gBufferPbr->attachTexture(GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, _gEmissionSpec);
	_gBufferPbr->attachTexture(GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, _gMetalRoughAO);
	_gBufferPbr->attachTexture(GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, _gIrradiance);
	_gBufferPbr->attachTexture(GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, _gPrefilter);
	_gBufferPbr->attachTexture(GL_COLOR_ATTACHMENT7, GL_TEXTURE_2D, _gReflectance);
	_gBufferPbr->attachRenderbuffer(GL_DEPTH_ATTACHMENT, _gRBO);

	//tell OpenGL which attachments the gBuffer will use for rendering
	unsigned int attachmentsPbr[8] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7};
	glDrawBuffers(8, attachmentsPbr);

	//check for completion
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: gBufferPbr framebuffer is incomplete." << std::endl;
	}

	//unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::_initConversionFBO() {
	//init conversion RBO
	_conversionRBO = new Renderbuffer();
	_conversionRBO->bind();
	_conversionRBO->init(GL_DEPTH_COMPONENT24, RenderSettings::SkyboxWidth, RenderSettings::SkyboxHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//init conversion FBO
	_conversionFBO = new Framebuffer();
	_conversionFBO->bind(GL_FRAMEBUFFER);
	_conversionFBO->attachRenderbuffer(GL_DEPTH_ATTACHMENT, _conversionRBO);

	//unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::_initShadowFBO() {
	//create shadow texture
	_shadowMap = new Texture();
	_shadowMap->bind(GL_TEXTURE_2D);
	_shadowMap->init(GL_TEXTURE_2D, GL_DEPTH_COMPONENT24, RenderSettings::ShadowWidth, RenderSettings::ShadowHeight, GL_DEPTH_COMPONENT, GL_FLOAT, NULL); //we only need the depth component
	_shadowMap->filter(GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_BORDER);

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

void Renderer::_initShadowCubeFBO() {
	//create shadow cubemap framebuffer
	_shadowCubeFBO = new Framebuffer();
	_shadowCubeFBO->bind(GL_FRAMEBUFFER);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	//create shadow cubemap texture for each possible light
	for(unsigned int i = 0; i < RenderSettings::MaxCubeShadows; i++) {
		_shadowCubeMaps.push_back(new Texture()); //add to vector

		_shadowCubeMaps[i]->bind(GL_TEXTURE_CUBE_MAP);

		//init all cubemap faces
		for(unsigned int j = 0; j < 6; j++) {
			_shadowCubeMaps[i]->init(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, GL_DEPTH_COMPONENT24, RenderSettings::ShadowWidth, RenderSettings::ShadowHeight, GL_DEPTH_COMPONENT, GL_FLOAT, NULL); //we only need the depth component
		}

		_shadowCubeMaps[i]->filter(GL_TEXTURE_CUBE_MAP, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE);
	}

	//bind back to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::_initDepthFBO() {
	//create shadow texture
	_sceneDepthBuffer = new Texture();
	_sceneDepthBuffer->bind(GL_TEXTURE_2D);
	_sceneDepthBuffer->init(GL_TEXTURE_2D, GL_DEPTH_COMPONENT32, Window::ScreenWidth, Window::ScreenHeight, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	_sceneDepthBuffer->filter(GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_BORDER);

	//create shadow framebuffer and attach the shadow map to it, so the framebuffer can render to it
	_depthFBO = new Framebuffer();
	_depthFBO->bind(GL_FRAMEBUFFER);
	_depthFBO->attachTexture(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _sceneDepthBuffer);
	glDrawBuffer(GL_NONE); //explicitly tell OpenGL that we are only using the depth attachments and no color attachments, otherwise the FBO will be incomplete
	glReadBuffer(GL_NONE);

	//check for completion
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: Scene depth framebuffer is incomplete." << std::endl;
	}

	//bind back to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::_initEnvironmentFBO() {
	//create renderbuffer
	_environmentRBO = new Renderbuffer();
	_environmentRBO->bind();
	_environmentRBO->init(GL_DEPTH_COMPONENT, RenderSettings::EnvironmentWidth, RenderSettings::EnvironmentHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//create framebuffer and only attach the renderbuffer as we attach each cubemap face seperately when we render the cubemap
	_environmentFBO = new Framebuffer();
	_environmentFBO->bind(GL_FRAMEBUFFER);
	_environmentFBO->attachRenderbuffer(GL_DEPTH_ATTACHMENT, _environmentRBO);

	//bind back to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::_initHdrFBO() {
	//create floating point color buffer
	_sceneColorBuffer = new Texture();
	_sceneColorBuffer->bind(GL_TEXTURE_2D);
	_sceneColorBuffer->init(GL_TEXTURE_2D, GL_RGBA16F, Window::ScreenWidth, Window::ScreenHeight, GL_RGBA, GL_FLOAT, NULL);
	_sceneColorBuffer->filter(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_BORDER);

	//create floating point bright color buffer
	_brightColorBuffer = new Texture();
	_brightColorBuffer->bind(GL_TEXTURE_2D);
	_brightColorBuffer->init(GL_TEXTURE_2D, GL_RGB16F, Window::ScreenWidth, Window::ScreenHeight, GL_RGB, GL_FLOAT, NULL);
	_brightColorBuffer->filter(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_BORDER);

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
		_blurColorBuffers[i]->init(GL_TEXTURE_2D, GL_RGBA16F, Window::ScreenWidth, Window::ScreenHeight, GL_RGBA, GL_FLOAT, NULL);
		_blurColorBuffers[i]->filter(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);

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
	//create ssao color buffer
	_ssaoColorBuffer = new Texture();
	_ssaoColorBuffer->bind(GL_TEXTURE_2D);
	_ssaoColorBuffer->init(GL_TEXTURE_2D, GL_RED, Window::ScreenWidth, Window::ScreenHeight, GL_RGB, GL_FLOAT, NULL);
	_ssaoColorBuffer->filter(GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST, GL_NONE);

	//create ssao framebuffer
	_ssaoFBO = new Framebuffer();
	_ssaoFBO->bind(GL_FRAMEBUFFER);
	_ssaoFBO->attachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ssaoColorBuffer); //attach buffer to fbo

	//check for completion
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: SSAO framebuffer is incomplete." << std::endl;
	}

	//create ssao blur color buffer
	_ssaoBlurColorBuffer = new Texture();
	_ssaoBlurColorBuffer->bind(GL_TEXTURE_2D);
	_ssaoBlurColorBuffer->init(GL_TEXTURE_2D, GL_RED, Window::ScreenWidth, Window::ScreenHeight, GL_RGB, GL_FLOAT, NULL);
	_ssaoBlurColorBuffer->filter(GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST, GL_NONE);

	//create ssao blur framebuffer
	_ssaoBlurFBO = new Framebuffer();
	_ssaoBlurFBO->bind(GL_FRAMEBUFFER);
	_ssaoBlurFBO->attachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ssaoBlurColorBuffer); //attach buffer to fbo

	//check for completion
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: SSAO blur framebuffer is incomplete." << std::endl;
	}

	//bind back to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::_initSSRFBO() {
	//create color buffer for the ssr texture
	_ssrColorBuffer = new Texture();
	_ssrColorBuffer->bind(GL_TEXTURE_2D);
	_ssrColorBuffer->init(GL_TEXTURE_2D, GL_RGBA16F, Window::ScreenWidth, Window::ScreenHeight, GL_RGBA, GL_FLOAT, NULL);
	_ssrColorBuffer->filter(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR, GL_NONE);

	//create ssr framebuffer
	_ssrFBO = new Framebuffer();
	_ssrFBO->bind(GL_FRAMEBUFFER);
	_ssrFBO->attachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ssrColorBuffer);

	//check for completion
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: SSR blur framebuffer is incomplete." << std::endl;
	}

	//bind back to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Texture* Renderer::_renderEnvironmentMap(std::vector<std::pair<RenderComponent*, glm::mat4>>& renderComponents, glm::mat4& environmentProjection, glm::vec3& renderPos, Texture* skybox, LightComponent* dirLight, bool pbr) {
	//create environment cubemap
	Texture* environmentMap = new Texture();
	environmentMap->bind(GL_TEXTURE_CUBE_MAP);

	//init all cubemap faces
	for(unsigned int i = 0; i < 6; i++) {
		environmentMap->init(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, GL_RGB16F, RenderSettings::EnvironmentWidth, RenderSettings::EnvironmentHeight, GL_RGB, GL_FLOAT, NULL); //hdr
	}

	if(pbr) {
		environmentMap->filter(GL_TEXTURE_CUBE_MAP, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
		environmentMap->generateMipmaps(GL_TEXTURE_CUBE_MAP);
	} else {
		environmentMap->filter(GL_TEXTURE_CUBE_MAP, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE);
	}

	//view matrices for each cubemap face
	std::vector<glm::mat4> environmentViews;
	environmentViews.push_back(glm::lookAt(renderPos, renderPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f))); //right
	environmentViews.push_back(glm::lookAt(renderPos, renderPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f))); //left
	environmentViews.push_back(glm::lookAt(renderPos, renderPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f))); //top
	environmentViews.push_back(glm::lookAt(renderPos, renderPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f))); //bottom
	environmentViews.push_back(glm::lookAt(renderPos, renderPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f))); //front
	environmentViews.push_back(glm::lookAt(renderPos, renderPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))); //back

	//forward render the scene objects without any special effects for each face
	glm::mat4 modelMatrix;
	Material* material;
	Model* model;

	for(unsigned int i = 0; i < 6; i++) {
		//attach respective face to render to to the framebuffer
		_environmentFBO->attachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, environmentMap);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//setup shader matrix
		_environmentShader->use();
		_environmentShader->setVec3("renderPos", renderPos);

		_environmentShader->setBool("useLight", dirLight != nullptr);

		if(dirLight != nullptr) {
			_environmentShader->setVec3("lightDirection", dirLight->lightDirection);
			_environmentShader->setVec3("lightAmbient", dirLight->lightAmbient);
			_environmentShader->setVec3("lightDiffuse", dirLight->lightDiffuse);
		}

		_environmentShader->setMat4("viewMatrix", environmentViews[i]);
		_environmentShader->setMat4("projectionMatrix", environmentProjection);

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

	return environmentMap;
}

Texture* Renderer::_renderIrradianceMap(Texture* environmentMap, glm::mat4& irradianceProjection) {
	//create environment cubemap
	Texture* irradianceMap = new Texture();
	irradianceMap->bind(GL_TEXTURE_CUBE_MAP);

	//init all cubemap faces
	for(unsigned int i = 0; i < 6; i++) {
		irradianceMap->init(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, GL_RGB16F, RenderSettings::IrradianceWidth, RenderSettings::IrradianceHeight, GL_RGB, GL_FLOAT, NULL); //hdr
	}

	irradianceMap->filter(GL_TEXTURE_CUBE_MAP, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);

	//view matrices for each cubemap face
	std::vector<glm::mat4> irradianceViews;
	irradianceViews.push_back(glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f))); //right
	irradianceViews.push_back(glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f))); //left
	irradianceViews.push_back(glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f))); //top
	irradianceViews.push_back(glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f))); //bottom
	irradianceViews.push_back(glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f))); //front
	irradianceViews.push_back(glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))); //back

	//rescale renderbuffer to irradiance dimensions
	glViewport(0, 0, RenderSettings::IrradianceWidth, RenderSettings::IrradianceHeight);
	_environmentRBO->bind();
	_environmentRBO->init(GL_DEPTH_COMPONENT, RenderSettings::IrradianceWidth, RenderSettings::IrradianceHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//set shader uniforms
	_irradianceShader->use();
	_irradianceShader->setMat4("projectionMatrix", irradianceProjection);

	//attach environment cubemap
	glActiveTexture(GL_TEXTURE0);
	environmentMap->bind(GL_TEXTURE_CUBE_MAP);

	//render each cubemap face
	for(unsigned int i = 0; i < 6; i++) {
		//attach respective face to render to to the framebuffer
		_environmentFBO->attachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//set view matrix for the face
		_irradianceShader->setMat4("viewMatrix", irradianceViews[i]);

		//render cube
		_skyboxVAO->bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	return irradianceMap;
}

Texture* Renderer::_renderPrefilterMap(Texture* environmentMap, glm::mat4& prefilterProjection) {
	//create environment cubemap
	Texture* prefilterMap = new Texture();
	prefilterMap->bind(GL_TEXTURE_CUBE_MAP);

	//init all cubemap faces
	for(unsigned int i = 0; i < 6; i++) {
		prefilterMap->init(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, GL_RGB16F, RenderSettings::PrefilterWidth, RenderSettings::PrefilterHeight, GL_RGB, GL_FLOAT, NULL); //hdr
	}

	prefilterMap->filter(GL_TEXTURE_CUBE_MAP, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
	prefilterMap->generateMipmaps(GL_TEXTURE_CUBE_MAP);

	//view matrices for each cubemap face
	std::vector<glm::mat4> prefilterViews;
	prefilterViews.push_back(glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f))); //right
	prefilterViews.push_back(glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f))); //left
	prefilterViews.push_back(glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f))); //top
	prefilterViews.push_back(glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f))); //bottom
	prefilterViews.push_back(glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f))); //front
	prefilterViews.push_back(glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))); //back

	//set shader uniforms
	_prefilterShader->use();
	_prefilterShader->setMat4("projectionMatrix", prefilterProjection);

	_prefilterShader->setFloat("resolution", (float)RenderSettings::EnvironmentHeight);

	//attach environment cubemap
	glActiveTexture(GL_TEXTURE0);
	environmentMap->bind(GL_TEXTURE_CUBE_MAP);

	//render each mipmap level
	for(unsigned int mip = 0; mip < RenderSettings::MaxMipLevels; mip++) {
		//rescale renderbuffer to prefilter mipmap dimensions
		unsigned int mipWidth = (unsigned int)(RenderSettings::PrefilterWidth * std::pow(0.5f, mip));
		unsigned int mipHeight = (unsigned int)(RenderSettings::PrefilterHeight * std::pow(0.5f, mip));

		glViewport(0, 0, mipWidth, mipHeight);
		_environmentRBO->bind();
		_environmentRBO->init(GL_DEPTH_COMPONENT, mipWidth, mipHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		//render each cubemap face
		float roughness = (float)mip / (float)(RenderSettings::MaxMipLevels - 1);
		_prefilterShader->setFloat("roughness", roughness);

		for(unsigned int i = 0; i < 6; i++) {
			//attach respective face to render to to the framebuffer
			_environmentFBO->attachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//set view matrix for the face
			_prefilterShader->setMat4("viewMatrix", prefilterViews[i]);

			//render cube
			_skyboxVAO->bind();
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}
	}

	return prefilterMap;
}

void Renderer::_renderBrdfLUT() {
	//create 2 channel lookup texture (LUT)
	_brdfLUT = new Texture();
	_brdfLUT->bind(GL_TEXTURE_2D);
	_brdfLUT->init(GL_TEXTURE_2D, GL_RG16F, RenderSettings::EnvironmentWidth, RenderSettings::EnvironmentHeight, GL_RG, GL_FLOAT, NULL); //2 channel hdr
	_brdfLUT->filter(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);

	//rescale renderbuffer to LUT texture dimensions
	glViewport(0, 0, RenderSettings::EnvironmentWidth, RenderSettings::EnvironmentHeight);
	_environmentRBO->bind();
	_environmentRBO->init(GL_DEPTH_COMPONENT, RenderSettings::EnvironmentWidth, RenderSettings::EnvironmentHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//attach BRDF texture to framebuffer
	_environmentFBO->attachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _brdfLUT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//render quad
	_brdfShader->use();
	_screenQuadVAO->bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Renderer::_renderShadowMaps(std::vector<std::pair<RenderComponent*, glm::mat4>>& renderComponents, std::vector<glm::vec3>& pointLights, glm::mat4& lightSpaceMatrix) {
	//adjust viewport and bind to shadow framebuffer
	glViewport(0, 0, RenderSettings::ShadowWidth, RenderSettings::ShadowHeight);
	_shadowFBO->bind(GL_FRAMEBUFFER);
	glClear(GL_DEPTH_BUFFER_BIT);

	//setup shader uniforms
	_shadowShader->use();
	_shadowShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	RenderComponent* renderComponent;
	Material* material;
	MaterialType materialType;
	Model* model;
	glm::mat4 modelMatrix;

	bool deferred = RenderSettings::IsEnabled(RenderSettings::Deferred);
	bool pbr = RenderSettings::IsEnabled(RenderSettings::PBR);

	//render all models' depth into the shadow map from the lights perspective
	for(unsigned int i = 0; i < renderComponents.size(); i++) {
		renderComponent = renderComponents[i].first;
		material = renderComponent->material;
		materialType = material->getMaterialType();

		if(deferred) {
			if(pbr && materialType != MaterialType::PBR) continue; //skip non-pbr materials in pbr mode
			else if(!pbr && materialType == MaterialType::PBR) continue; //skip pbr materials in non-pbr mode
		}

		modelMatrix = renderComponents[i].second;
		model = renderComponent->model;

		if(!material->getCastsShadows()) continue; //skip this model, if it should not cast shadows

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
		_shadowCubeFBO->bind(GL_FRAMEBUFFER);
		_shadowCubeFBO->attachCubemap(GL_DEPTH_ATTACHMENT, _shadowCubeMaps[i]); //attach the correct shadow cubemap as depth attachment to the framebuffer before rendering
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
			renderComponent = renderComponents[i].first;
			material = renderComponent->material;
			materialType = material->getMaterialType();

			if(deferred) {
				if(pbr && materialType != MaterialType::PBR) continue; //skip non-pbr materials in pbr mode in deferred mode
				else if(!pbr && materialType == MaterialType::PBR) continue; //skip pbr material in non-pbr mode in deferred mode
			}

			modelMatrix = renderComponents[i].second;
			model = renderComponent->model;

			if(!material->getCastsShadows()) continue; //skip this model, if it should not cast shadows (e.g. like glass)

			_shadowCubeShader->setMat4("modelMatrix", modelMatrix);
			model->draw();
		}
	}

	//reset viewport
	glViewport(0, 0, Window::ScreenWidth, Window::ScreenHeight);
}

void Renderer::_renderDepth(std::vector<std::pair<RenderComponent*, glm::mat4>>& renderComponents) {
	//render the depth texture seperately to enable rendering thickness maps if needed lateron

	//bind to depth framebuffer
	_depthFBO->bind(GL_FRAMEBUFFER);
	glClear(GL_DEPTH_BUFFER_BIT);

	//setup shader uniforms
	_depthShader->use();

	RenderComponent* renderComponent;
	Material* material;
	MaterialType materialType;
	Model* model;
	glm::mat4 modelMatrix;

	bool deferred = RenderSettings::IsEnabled(RenderSettings::Deferred);
	bool pbr = RenderSettings::IsEnabled(RenderSettings::PBR);

	//render all models' depth into the shadow map from the lights perspective
	for(unsigned int i = 0; i < renderComponents.size(); i++) {
		renderComponent = renderComponents[i].first;
		material = renderComponent->material;
		materialType = material->getMaterialType();

		if(deferred) {
			if(pbr && materialType != MaterialType::PBR) continue; //skip non-pbr materials in pbr mode in deferred mode
			else if(!pbr && materialType == MaterialType::PBR) continue; //skip pbr material in non-pbr mode in deferred mode
		}

		modelMatrix = renderComponents[i].second;
		model = renderComponent->model;

		_depthShader->setMat4("modelMatrix", modelMatrix);
		model->draw();
	}
}

void Renderer::_renderGeometry(std::vector<std::pair<RenderComponent*, glm::mat4>>& solidRenderComponents, bool pbr) {
	//bind to gBuffer framebuffer and render to buffer textures
	if(pbr) _gBufferPbr->bind(GL_FRAMEBUFFER);
	else _gBuffer->bind(GL_FRAMEBUFFER);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//SSR
	GLfloat clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	glClearTexImage(_gEmissionSpec->getID(), 0, GL_RG, GL_FLOAT, &clearColor); //initialize the emission spec with 0 to avoid having specularity on the skybox

	MaterialType materialType;
	RenderComponent* renderComponent;
	Material* material;
	glm::mat4 modelMatrix;
	Model* model;

	for(unsigned int i = 0; i < solidRenderComponents.size(); i++) {
		renderComponent = solidRenderComponents[i].first;
		material = renderComponent->material;
		materialType = material->getMaterialType();

		if(materialType == MaterialType::PBR && !pbr) continue; //skip pbr materials in non pbr mode
		else if(materialType != MaterialType::PBR && pbr) continue; //skip non-pbr material in pbr mode

		modelMatrix = solidRenderComponents[i].second;
		model = renderComponent->model;

		//bind irradiance and prefilter map
		if(_iblMaps.count(renderComponent)) {
			glActiveTexture(GL_TEXTURE8);
			_iblMaps[renderComponent].irradianceMap->bind(GL_TEXTURE_CUBE_MAP);

			glActiveTexture(GL_TEXTURE9);
			_iblMaps[renderComponent].prefilterMap->bind(GL_TEXTURE_CUBE_MAP);
		}

		//bind environment map
		if(_environmentMaps.count(renderComponent)) {
			glActiveTexture(GL_TEXTURE8);
			_environmentMaps[renderComponent]->bind(GL_TEXTURE_CUBE_MAP);
		}

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
	_gPosition->bind(GL_TEXTURE_2D);

	//bind normal color buffer
	glActiveTexture(GL_TEXTURE1);
	_gNormal->bind(GL_TEXTURE_2D);

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

void Renderer::_renderSSR(CameraComponent* cameraComponent) {
	//bind to ssr framebuffer
	_ssrFBO->bind(GL_FRAMEBUFFER);
	glClear(GL_COLOR_BUFFER_BIT);

	//setup shader uniforms
	_ssrShader->use();

	_ssrShader->setFloat("rayStepSize", RenderSettings::SsrRayStepSize);
	_ssrShader->setFloat("maxRaySteps", (int)RenderSettings::SsrMaxRaySteps);
	_ssrShader->setFloat("fresnelExponent", RenderSettings::SsrFresnelExponent);
	_ssrShader->setFloat("maxDelta", RenderSettings::SsrMaxDelta);

	//bind normal color buffer
	glActiveTexture(GL_TEXTURE0);
	_gNormal->bind(GL_TEXTURE_2D);

	//bind albedo color buffer 
	glActiveTexture(GL_TEXTURE1);
	_gEmissionSpec->bind(GL_TEXTURE_2D);

	//bind scene texture
	glActiveTexture(GL_TEXTURE2);
	_sceneColorBuffer->bind(GL_TEXTURE_2D);

	//bind scene texture
	glActiveTexture(GL_TEXTURE3);
	_sceneDepthBuffer->bind(GL_TEXTURE_2D);

	//render quad
	_screenQuadVAO->bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	//bind back to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::_renderLighting(Texture* skybox, unsigned int pointLightCount, bool pbr) {
	//bind to hdr framebuffer
	_hdrFBO->bind(GL_FRAMEBUFFER);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(pbr) {
		//use lighting shader and bind textures
		_lightingShaderPbr->use();

		_lightingShaderPbr->setBool("useSSAO", RenderSettings::IsEnabled(RenderSettings::SSAO));

		//bind position color buffer
		glActiveTexture(GL_TEXTURE0);
		_gPosition->bind(GL_TEXTURE_2D);

		//bind normal color buffer
		glActiveTexture(GL_TEXTURE1);
		_gNormal->bind(GL_TEXTURE_2D);

		//bind albedo color buffer
		glActiveTexture(GL_TEXTURE2);
		_gAlbedo->bind(GL_TEXTURE_2D);

		//bind emission + specular color buffer
		glActiveTexture(GL_TEXTURE3);
		_gEmissionSpec->bind(GL_TEXTURE_2D);

		//bind metallic + roughness + ao color buffer
		glActiveTexture(GL_TEXTURE4);
		_gMetalRoughAO->bind(GL_TEXTURE_2D);

		//bind irradiance color buffer
		glActiveTexture(GL_TEXTURE5);
		_gIrradiance->bind(GL_TEXTURE_2D);

		//bind prefilter color buffer
		glActiveTexture(GL_TEXTURE6);
		_gPrefilter->bind(GL_TEXTURE_2D);

		//bind reflectance (F0) color buffer
		glActiveTexture(GL_TEXTURE7);
		_gReflectance->bind(GL_TEXTURE_2D);

		//bind brdf LUT
		glActiveTexture(GL_TEXTURE9);
		_brdfLUT->bind(GL_TEXTURE_2D);
	} else {
		//use lighting shader and bind textures
		_lightingShader->use();

		_lightingShader->setBool("useSSAO", RenderSettings::IsEnabled(RenderSettings::SSAO));

		//bind position color buffer
		glActiveTexture(GL_TEXTURE0);
		_gPosition->bind(GL_TEXTURE_2D);

		//bind normal color buffer
		glActiveTexture(GL_TEXTURE1);
		_gNormal->bind(GL_TEXTURE_2D);

		//bind albedo color buffer
		glActiveTexture(GL_TEXTURE2);
		_gAlbedo->bind(GL_TEXTURE_2D);

		//bind emission + specular color buffer
		glActiveTexture(GL_TEXTURE3);
		_gEmissionSpec->bind(GL_TEXTURE_2D);

		//bind environment + shininess color buffer
		glActiveTexture(GL_TEXTURE4);
		_gEnvironmentShiny->bind(GL_TEXTURE_2D);
	}

	//bind ssao texture
	glActiveTexture(GL_TEXTURE8);
	_ssaoBlurColorBuffer->bind(GL_TEXTURE_2D);

	//bind shadow map
	glActiveTexture(GL_TEXTURE11);
	_shadowMap->bind(GL_TEXTURE_2D);

	//bind shadow cubemap
	for(unsigned int i = 0; i < pointLightCount; i++) {
		glActiveTexture(GL_TEXTURE12 + i);
		_shadowCubeMaps[i]->bind(GL_TEXTURE_CUBE_MAP);
	}

	//render quad
	_screenQuadVAO->bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Renderer::_renderScene(std::vector<std::pair<RenderComponent*, glm::mat4>>& renderComponents, unsigned int pointLightCount, bool useShadows, bool bindFBO) {
	//bind to hdr framebuffer if needed and render each renderable 
	if(bindFBO) {
		_hdrFBO->bind(GL_FRAMEBUFFER);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	//bind shadow maps
	if(useShadows) {
		glActiveTexture(GL_TEXTURE11);
		_shadowMap->bind(GL_TEXTURE_2D);

		for(unsigned int i = 0; i < pointLightCount; i++) {
			glActiveTexture(GL_TEXTURE12 + i);
			_shadowCubeMaps[i]->bind(GL_TEXTURE_CUBE_MAP);
		}
	}

	RenderComponent* renderComponent;
	MaterialType materialType;
	Material* material;
	Model* model;
	glm::mat4 modelMatrix;

	bool deferredPBR = RenderSettings::IsEnabled(RenderSettings::PBR | RenderSettings::Deferred); //if pbr mode and deferred is active (true when rendering blen objects in deferred mode)

	for(unsigned int i = 0; i < renderComponents.size(); i++) {
		renderComponent = renderComponents[i].first;
		material = renderComponent->material;
		materialType = material->getMaterialType();

		if(deferredPBR && materialType != MaterialType::PBR) continue; //dont render non-pbr blend objects

		model = renderComponent->model;
		modelMatrix = renderComponents[i].second;


		//bind irradiance cubemap, prefilter cubemap and brdfLUT
		if(_iblMaps.count(renderComponent)) {
			glActiveTexture(GL_TEXTURE8);
			_iblMaps[renderComponent].irradianceMap->bind(GL_TEXTURE_CUBE_MAP);

			glActiveTexture(GL_TEXTURE9);
			_iblMaps[renderComponent].prefilterMap->bind(GL_TEXTURE_CUBE_MAP);

			glActiveTexture(GL_TEXTURE10);
			_brdfLUT->bind(GL_TEXTURE_2D);
		}

		//bind environment cubemap
		if(_environmentMaps.count(renderComponent)) {
			glActiveTexture(GL_TEXTURE8);
			_environmentMaps[renderComponent]->bind(GL_TEXTURE_CUBE_MAP);
		}

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
	_postProcessingShader->setBool("useMotionBlur", RenderSettings::IsEnabled(RenderSettings::MotionBlur));
	_postProcessingShader->setBool("useSSR", RenderSettings::IsEnabled(RenderSettings::SSR));

	_postProcessingShader->setBool("ssrDebug", RenderSettings::SsrDebug);

	_postProcessingShader->setInt("motionBlurSamples", RenderSettings::MotionBlurSamples);
	_postProcessingShader->setFloat("velocityScale", RenderSettings::VelocityScale);

	_postProcessingShader->setFloat("gamma", RenderSettings::Gamma);
	_postProcessingShader->setFloat("exposure", RenderSettings::Exposure);

	//set uniforms for FXAA
	_postProcessingShader->setVec2("inverseScreenTextureSize", glm::vec2(1.0f / (float)Window::ScreenWidth, 1.0f / (float)Window::ScreenHeight));
	_postProcessingShader->setFloat("fxaaSpanMax", RenderSettings::FxaaSpanMax);
	_postProcessingShader->setFloat("fxaaReduceMin", RenderSettings::FxaaReduceMin);
	_postProcessingShader->setFloat("fxaaReduceMul", RenderSettings::FxaaReduceMul);

	//bind textures
	glActiveTexture(GL_TEXTURE0);
	_sceneColorBuffer->bind(GL_TEXTURE_2D); //bind screen texture

	glActiveTexture(GL_TEXTURE1);
	_sceneDepthBuffer->bind(GL_TEXTURE_2D); //bind scene depth texture

	glActiveTexture(GL_TEXTURE2);
	_blurColorBuffers[!horizontal]->bind(GL_TEXTURE_2D); //bind blurred bloom texture

	glActiveTexture(GL_TEXTURE3);
	_ssrColorBuffer->bind(GL_TEXTURE_2D); //bind the ssr reflection texture

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

void Renderer::_fillUniformBuffers(glm::mat4& viewMatrix, glm::mat4& projectionMatrix, glm::mat4& previousViewProjection, glm::mat4& lightSpaceMatrix, glm::vec3& cameraPos, glm::vec3& directionalLightPos, bool useShadows, std::vector<glm::vec3>& pointLightPositions) {
	//store the matrices in the matrices uniform buffer
	_matricesUBO->bind(GL_UNIFORM_BUFFER);
	_matricesUBO->bufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMatrix)); //buffer view matrix
	_matricesUBO->bufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projectionMatrix)); //buffer projection matrix
	_matricesUBO->bufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(previousViewProjection)); //previous view projection matrix matrix
	_matricesUBO->bufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(lightSpaceMatrix)); //buffer light space matrix

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

void Renderer::_blitGDepthToHDR(bool pbr) {
	//blit the depth buffer of the gBuffer into the depth buffer of the hdr fbo
	if(pbr) _gBufferPbr->bind(GL_READ_FRAMEBUFFER);
	else _gBuffer->bind(GL_READ_FRAMEBUFFER);
	_hdrFBO->bind(GL_DRAW_FRAMEBUFFER);

	glBlitFramebuffer(0, 0, Window::ScreenWidth, Window::ScreenHeight, 0, 0, Window::ScreenWidth, Window::ScreenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

void Renderer::_updateDimensions() {
	if(!Window::DimensionsChanged) return;

	//first free all ressources that depend on the screen dimensions to avoid memory leaks
	delete _gBuffer;
	delete _gBufferPbr;
	delete _depthFBO;
	delete _hdrFBO;
	delete _bloomBlurFBOs[0];
	delete _bloomBlurFBOs[1];
	delete _ssaoFBO;
	delete _ssaoBlurFBO;
	delete _ssrFBO;

	delete _gRBO;
	delete _hdrRBO;

	delete _gPosition;
	delete _gNormal;
	delete _gAlbedo;
	delete _gEmissionSpec;

	delete _gEnvironmentShiny;

	delete _gMetalRoughAO;
	delete _gIrradiance;
	delete _gPrefilter;
	delete _gReflectance;

	delete _sceneDepthBuffer;
	delete _sceneColorBuffer;
	delete _brightColorBuffer;
	delete _blurColorBuffers[0];
	delete _blurColorBuffers[1];
	delete _ssaoColorBuffer;
	delete _ssaoBlurColorBuffer;
	delete _ssrColorBuffer;

	//then reallocate them with the correct new dimensions
	_initGBuffers();
	_initDepthFBO();
	_initHdrFBO();
	_initBlurFBOs();
	_initSSAOFBOs();
	_initSSRFBO();

	Window::DimensionsChanged = false; //reset
}

void Renderer::_updateVSync() {
	if(RenderSettings::VSync == _vSync) return;

	_vSync = RenderSettings::VSync;

	if(_vSync) {
		glfwSwapInterval(1); //enable vsync
		_vSync = true;
	} else {
		glfwSwapInterval(0); //disable vysnc
		_vSync = false;
	}
}