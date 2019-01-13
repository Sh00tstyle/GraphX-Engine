#ifndef DEMOSCENE1_H
#define DEMOSCENE1_H

#include "../Engine/Scene.h"

class DemoScene1: public Scene {
	public:
		DemoScene1();
		~DemoScene1();

		virtual void initializeScene(World* world, SceneManager* manager);

};

#endif