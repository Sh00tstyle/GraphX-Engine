#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <vector>
#include <bitset>

class Window;
class World;
class Node;
class Texture;
class Renderer;
class OverlayUI;
class Debug;
class Scene;

class SceneManager {
	public:
		SceneManager();
		~SceneManager();

		void queueScene(int index);
		void addScene(Scene* scene);

		void setSkybox(Texture* skybox, bool isEquirectangular = false);
		void setMainCamera(Node* mainCamera);
		void setDirectionalLight(Node* directionalLight);

		virtual void initialize(unsigned int sceneIndex);
		virtual void run();

	protected:
		Window* _window;
		World* _world;
		Renderer* _renderer;
		OverlayUI* _ui;

		virtual void _update();
		virtual void _render();

	private:
		Texture * _skybox;
		Node* _mainCamera;
		Node* _directionalLight;

		Debug* _profiler;

		std::vector<Node*> _renderables;
		std::vector<Node*> _lights;

		std::vector<Scene*> _scenes;

		int _queuedSceneIndex;

		void _loadScene();
		void _initializeEnvironmentMaps();

};

#endif