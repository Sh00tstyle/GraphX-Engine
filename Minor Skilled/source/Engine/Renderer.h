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
		Renderer();
		~Renderer();

		static std::bitset<8> Settings; //probably needs more than 8 bits eventually

		void render(std::vector<Node*>& renderables, std::vector<Node*>& lights, Node* mainCamera, Node* directionalLight, Texture* skybox);

	private:
		//configurations
		static const unsigned int _ShadowWidth;
		static const unsigned int _ShadowHeight;

		//vertex data
		static const float _SkyboxVertices[];
		static const float _ScreenQuadVertices[];

		//shaders
		Shader* _shadowShader;
		Shader* _skyboxShader;
		Shader* _screenQuadShader;

		//VAOs, VBOs
		unsigned int _skyboxVAO;
		unsigned int _skyboxVBO;

		unsigned int _screenQuadVAO;
		unsigned int _screenQuadVBO;

		//FBOs, RBOs
		unsigned int _shadowFBO;

		//texture buffers
		Texture* _shadowMap;

		//init functions
		void _initShaders();

		void _initSkyboxVAO();
		void _initScreenQuadVAO();

		void _initShadowFBO();
		
		//render functions
		void _renderShadowMap(std::vector<std::pair<RenderComponent*, glm::mat4>>& renderComponents, glm::mat4& lightSpaceMatrix);
		void _renderScene(glm::mat4& viewMatrix, glm::mat4& projectionMatrix, glm::mat4& lightSpaceMatrix, glm::vec3& cameraPos, glm::vec3& directionalLightPos, std::vector<std::pair<LightComponent*, glm::vec3>>& lightComponents, std::vector<std::pair<RenderComponent*, glm::mat4>>& renderComponents);
		void _renderSkybox(glm::mat4& viewMatrix, glm::mat4& projectionMatrix, Texture* skybox);
		void _renderScreenQuad(Texture* screenTexture);

		//helper functions
		std::vector<std::pair<RenderComponent*, glm::mat4>> _getSortedRenderComponents(std::vector<Node*>& renderables, glm::vec3& cameraPos);

};

#endif