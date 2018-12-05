#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <bitset>

#include <glm\glm.hpp>

class Node;
class Shader;
class Texture;
class RenderComponent;
class LightComponent;
class VertexArray;
class Buffer;
class Framebuffer;
class Renderbuffer;

class Renderer {
	public:
		Renderer();
		~Renderer();

		void render(std::vector<Node*>& renderables, std::vector<Node*>& lights, Node* mainCamera, Node* directionalLight, Texture* skybox);

	private:
		//vertex data
		static const std::vector<float> _SkyboxVertices;
		static const std::vector<float> _ScreenQuadVertices;

		//shaders
		Shader* _lightingShader;
		Shader* _shadowShader;
		Shader* _shadowCubeShader;
		Shader* _environmentShader;
		Shader* _irradianceShader;
		Shader* _skyboxShader;
		Shader* _ssaoShader;
		Shader* _ssaoBlurShader;
		Shader* _bloomBlurShader;
		Shader* _postProcessingShader;

		//texture buffers
		Texture* _gPositionRefract;
		Texture* _gNormalReflect;
		Texture* _gAlbedoSpec;
		Texture* _gEmissionShiny;

		Texture* _shadowMap;
		std::vector<Texture*> _shadowCubeMaps;
		Texture* _environmentMap;
		Texture* _irradianceMap;
		Texture* _sceneColorBuffer;
		Texture* _brightColorBuffer;
		Texture* _blurColorBuffers[2];

		Texture* _ssaoNoiseTexture;
		Texture* _ssaoColorBuffer;
		Texture* _ssaoBlurColorBuffer;

		//VAOs, VBOs
		VertexArray* _skyboxVAO;
		VertexArray* _screenQuadVAO;

		Buffer* _skyboxVBO;
		Buffer* _screenQuadVBO;

		//UBOs, SSBOs
		Buffer* _matricesUBO;
		Buffer* _dataUBO;

		Buffer* _lightsSSBO;

		//FBOs, RBOs
		Framebuffer* _gBuffer;

		Framebuffer* _shadowFBO;
		std::vector<Framebuffer*> _shadowCubeFBOs;
		Framebuffer* _environmentFBO;
		Framebuffer* _irradianceFBO;
		Framebuffer* _hdrFBO;
		Framebuffer* _bloomFBO;
		Framebuffer* _bloomBlurFBOs[2];
		Framebuffer* _ssaoFBO;
		Framebuffer* _ssaoBlurFBO;

		Renderbuffer* _gRBO;

		Renderbuffer* _environmentRBO;
		Renderbuffer* _irradianceRBO;
		Renderbuffer* _hdrRBO;

		//kernels
		std::vector<glm::vec3> _ssaoKernel;

		//init functions
		void _initShaders();

		void _initSkyboxVAO();
		void _initScreenQuadVAO();

		void _initUniformBuffers();
		void _initShaderStorageBuffers();

		void _initGBuffer();
		void _initShadowFBO();
		void _initShadowCubeFBOs();
		void _initEnvironmentFBO();
		void _initIrradianceFBO();
		void _initHdrFBO();
		void _initBlurFBOs();
		void _initSSAOFBOs();
		
		//render functions
		void _renderShadowMaps(std::vector<std::pair<RenderComponent*, glm::mat4>>& renderComponents, std::vector<glm::vec3>& pointLights, glm::mat4& lightSpaceMatrix);
		void _renderEnvironmentMap(std::vector<std::pair<RenderComponent*, glm::mat4>>& renderComponents, Texture* skybox, glm::vec3& cameraPos, unsigned int pointLightCount);
		void _renderIrradianceMap(Texture* skybox);
		void _renderGeometry(std::vector<std::pair<RenderComponent*, glm::mat4>>& solidRenderComponents);
		void _renderSSAO();
		void _renderSSAOBlur();
		void _renderLighting(Texture* skybox, unsigned int pointLightCount);
		void _renderScene(std::vector<std::pair<RenderComponent*, glm::mat4>>& renderComponents, Texture* skybox, unsigned int pointLightCount, bool useShadows, bool bindFBO);
		void _renderSkybox(glm::mat4& viewMatrix, glm::mat4& projectionMatrix, Texture* skybox);
		void _renderPostProcessingQuad();

		//helper functions
		void _getSortedRenderComponents(std::vector<Node*>& renderables, glm::vec3& cameraPos, std::vector<std::pair<RenderComponent*, glm::mat4>>& solidRenderables, std::vector<std::pair<RenderComponent*, glm::mat4>>& blendRenderables);
		std::vector<glm::vec3> _getClosestPointLights(glm::vec3 cameraPos, std::vector<std::pair<LightComponent*, glm::vec3>>& lightComponents);
		glm::vec3 _getClosestReflection(std::vector<std::pair<RenderComponent*, glm::mat4>>& renderComponents, glm::vec3& cameraPos);

		void _fillUniformBuffers(glm::mat4& viewMatrix, glm::mat4& projectionMatrix, glm::mat4& lightSpaceMatrix, glm::vec3& cameraPos, glm::vec3& directionalLightPos, bool useShadows, std::vector<glm::vec3>& pointLightPositions);
		void _fillShaderStorageBuffers(std::vector<std::pair<LightComponent*, glm::vec3>>& lightComponents);

		void _generateSSAOKernel();
		void _generateNoiseTexture();

		void _blitGDepthToHDR();
};

#endif