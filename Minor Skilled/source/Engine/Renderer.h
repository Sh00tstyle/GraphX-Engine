#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

#include <glm\glm.hpp>

#include "../Utility/RenderSettings.h"

class Node;
class RenderComponent;
class Shader;
class Texture;

class Renderer {
	public:
		Renderer();
		~Renderer();

		static RenderSettings Settings;

		void render(std::vector<Node*>& renderables, std::vector<Node*>& lights, std::vector<Node*>& cameras, Texture* skybox);

	private:
		static Shader* _skyboxShader;
		static const float _skyboxVertices[];

		unsigned int _skyboxVAO;
		unsigned int _skyboxVBO;

		void _initSkyboxVAO();
		
		void _renderSkybox(glm::mat4& viewMatrix, glm::mat4& projectionMatrix, Texture* skybox);
		std::vector<std::pair<RenderComponent*, glm::mat4>> _getSortedRenderComponents(std::vector<Node*>& renderables, glm::vec3& cameraPos);

};

#endif