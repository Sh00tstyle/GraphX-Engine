#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <bitset>

#include "../Utility/ComponentType.h"

class Window;
class World;
class System;

class Scene {
	public:
		Scene();
		~Scene();

		virtual void initialize();
		virtual void run();

	protected:
		virtual void update();
		virtual void render();

		Window* _window;
		World* _world;
		std::vector<System*> _systems;

	private:
		const std::bitset<8> _renderMask;

		virtual void _initializeScene() = 0;

};

#endif