#ifndef DEMOSCENE2_H
#define DEMOSCENE2_H

#include "../Engine/Scene.h"

class DemoScene2: public Scene {
	public:
		DemoScene2();
		~DemoScene2();

		virtual void initializeScene(World* world, SceneManager* manager);

};

#endif