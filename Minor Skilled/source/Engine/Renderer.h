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
		Shader* _shadowShader;
		Shader* _skyboxShader;
		Shader* _blurShader;
		Shader* _postProcessingShader;

		//texture buffers
		Texture* _shadowMap;
		Texture* _multiSampledColorBuffer;
		Texture* _multiSampledBrightColorBuffer;
		Texture* _bloomBrightColorBuffer;
		Texture* _blurColorbuffers[2];

		//VAOs, VBOs
		unsigned int _skyboxVAO;
		unsigned int _skyboxVBO;

		unsigned int _screenQuadVAO;
		unsigned int _screenQuadVBO;

		//UBOs
		unsigned int _matricesUBO;
		unsigned int _dataUBO;
		unsigned int _lightsUBO;

		//FBOs, RBOs
		unsigned int _shadowFBO;
		unsigned int _multisampledHdrFBO;
		unsigned int _bloomFBO;
		unsigned int _blurFBOs[2];

		unsigned int _depthRBO;

		//init functions
		void _initShaders();

		void _initSkyboxVAO();
		void _initScreenQuadVAO();

		void _initUniformBuffers();

		void _initShadowFBO();
		void _initMultisampledHdrFBO();
		void _initBloomFBO();
		void _initBlurFBOs();
		
		//render functions
		void _renderShadowMap(std::vector<std::pair<RenderComponent*, glm::mat4>>& renderComponents, glm::mat4& lightSpaceMatrix);
		void _renderScene(std::vector<std::pair<RenderComponent*, glm::mat4>>& renderComponents);
		void _renderSkybox(glm::mat4& viewMatrix, glm::mat4& projectionMatrix, Texture* skybox);
		void _renderPostProcessingQuad(float gamma = 2.2f, float exposure = 1.0f);

		//helper functions
		void _fillUniformBuffers(glm::mat4& viewMatrix, glm::mat4& projectionMatrix, glm::mat4& lightSpaceMatrix, glm::vec3& cameraPos, glm::vec3& directionalLightPos, bool& useShadows, std::vector<std::pair<LightComponent*, glm::vec3>>& lightComponents);
		std::vector<std::pair<RenderComponent*, glm::mat4>> _getSortedRenderComponents(std::vector<Node*>& renderables, glm::vec3& cameraPos);

		void _blitHDRtoBloomFBO();

};

#endif