#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "../Engine/System.h"

class RenderSystem :public System {
	public:
		RenderSystem();
		~RenderSystem();

		virtual void render();

};

#endif