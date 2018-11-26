#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <bitset>

#include <glm\glm.hpp>

#include "../Utility/RenderSettings.h"

class Node;
class Shader;
class Texture;
class RenderComponent;
class LightComponent;

class Renderer {
	public:
		Renderer(unsigned int msaaSamples = 4);
		~Renderer();

		static std::bitset<8> Settings; //probably needs more than 8 bits eventually

		void render(std::vector<Node*>& renderables, std::vector<Node*>& lights, Node* mainCamera, Node* directionalLight, Texture* skybox);

	private:
		//configurations
		static const unsigned int _ShadowWidth;
		static const unsigned int _ShadowHeight;

		unsigned int _msaaSamples;

		//vertex data
		static const float _SkyboxVertices[];
		static const float _ScreenQuadVertices[];

		//shaders
		Shader* _lightingShader;
		Shader* _shadowShader;
		Shader* _skyboxShader;
		Shader* _ssaoShader;
		Shader* _ssaoBlurShader;
		Shader* _bloomBlurShader;
		Shader* _postProcessingShader;

		//texture buffers
		Texture* _gPosition;
		Texture* _gNormal;
		Texture* _gAlbedoSpec;
		Texture* _gEmissionShiny;

		Texture* _shadowMap;
		Texture* _multiSampledColorBuffer;
		Texture* _multiSampledBrightColorBuffer;
		Texture* _bloomBrightColorBuffer;
		Texture* _blurColorBuffers[2];
		Texture* _ssaoNoiseTexture;
		Texture* _ssaoColorBuffer;
		Texture* _ssaoBlurColorBuffer;

		//VAOs, VBOs
		unsigned int _skyboxVAO;
		unsigned int _skyboxVBO;

		unsigned int _screenQuadVAO;
		unsigned int _screenQuadVBO;

		//UBOs, SSBOs
		unsigned int _matricesUBO;
		unsigned int _dataUBO;

		unsigned int _lightsSSBO;

		//FBOs, RBOs
		unsigned int _gBuffer;

		unsigned int _shadowFBO;
		unsigned int _multisampledHdrFBO;
		unsigned int _bloomFBO;
		unsigned int _bloomBlurFBOs[2];
		unsigned int _ssaoFBO;
		unsigned int _ssaoBlurFBO;

		unsigned int _gRBO;

		unsigned int _multisampledHdrRBO;

		//Kernels
		std::vector<glm::vec3> _ssaoKernel;

		//init functions
		void _initShaders();

		void _initSkyboxVAO();
		void _initScreenQuadVAO();

		void _initUniformBuffers();
		void _initShaderStorageBuffers();

		void _initGBuffer();
		void _initShadowFBO();
		void _initMultisampledHdrFBO();
		void _initBloomFBOs();
		void _initSSAOFBOs();
		
		//render functions
		void _renderShadowMap(std::vector<std::pair<RenderComponent*, glm::mat4>>& renderComponents, glm::mat4& lightSpaceMatrix);
		void _renderSceneGeometry(std::vector<std::pair<RenderComponent*, glm::mat4>>& solidRenderComponents);
		void _renderSSAO();
		void _renderSSAOBlur();
		void _renderSceneLighting();
		void _renderScene(std::vector<std::pair<RenderComponent*, glm::mat4>>& renderComponents, bool bindFBO);
		void _renderSkybox(glm::mat4& viewMatrix, glm::mat4& projectionMatrix, Texture* skybox);
		void _renderPostProcessingQuad(float gamma = 2.2f, float exposure = 1.0f);

		//helper functions
		void _getSortedRenderComponents(std::vector<Node*>& renderables, glm::vec3& cameraPos, std::vector<std::pair<RenderComponent*, glm::mat4>>& solidRenderables, std::vector<std::pair<RenderComponent*, glm::mat4>>& blendRenderables);

		void _fillUniformBuffers(glm::mat4& viewMatrix, glm::mat4& projectionMatrix, glm::mat4& lightSpaceMatrix, glm::vec3& cameraPos, glm::vec3& directionalLightPos, bool& useShadows);
		void _fillShaderStorageBuffers(std::vector<std::pair<LightComponent*, glm::vec3>>& lightComponents);

		void _generateSSAOKernel();
		void _generateNoiseTexture();

		void _blitGDepthToHDR();
		void _blitHDRtoBloomFBO();

		float _lerp(float a, float b, float f); //move to math helper class

};

#endif