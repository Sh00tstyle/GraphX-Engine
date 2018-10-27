#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "../Engine/System.h"

class RenderSystem :public System {
	public:
		RenderSystem();
		~RenderSystem();

		virtual void render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

};

#endif