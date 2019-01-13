#ifndef SCENE_H
#define SCENE_H

class World;
class SceneManager;

class Scene {
	public:
		Scene();
		~Scene();

		virtual void initializeScene(World* world, SceneManager* manager) = 0;
};

#endif