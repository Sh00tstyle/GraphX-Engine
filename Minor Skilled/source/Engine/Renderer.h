#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

#include "../Utility/RenderSettings.h"

class Node;

class Renderer {
	public:
		Renderer();
		~Renderer();

		static RenderSettings Settings;

		void render(std::vector<Node*>& renderables, std::vector<Node*>& lights, std::vector<Node*>& cameras);

};

#endif