#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <bitset>

class Window;
class World;
class Node;
class Renderer;

class Scene {
	public:
		~Scene();

		virtual void initialize();
		virtual void run();

	protected:
		Scene();

		Window* _window;
		World* _world;
		Renderer* _renderer;

		virtual void _update();
		virtual void _render();

	private:
		std::vector<Node*> _renderables;
		std::vector<Node*> _lights;
		std::vector<Node*> _cameras;

		virtual void _initializeScene() = 0;

};

#endif