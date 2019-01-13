#ifndef DEMOSCENE4_H
#define DEMOSCENE4_H

#include "../Engine/Scene.h"

class DemoScene4: public Scene {
	public:
		DemoScene4();
		~DemoScene4();

		virtual void initializeScene(World* world, SceneManager* manager);

};

#endif