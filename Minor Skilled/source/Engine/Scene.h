#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <bitset>

class Window;
class World;
class Node;
class Texture;
class Renderer;
class OverlayUI;

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
		OverlayUI* _ui;

		void _setSkybox(Texture* skybox, bool isEquirectangular = false);
		void _setMainCamera(Node* mainCamera);
		void _setDirectionalLight(Node* directionalLight);

		virtual void _update();
		virtual void _render();

	private:
		Texture * _skybox;
		Node* _mainCamera;
		Node* _directionalLight;

		std::vector<Node*> _renderables;
		std::vector<Node*> _lights;

		void _initializeEnvironmentMaps();

		virtual void _initializeScene() = 0;

};

#endif