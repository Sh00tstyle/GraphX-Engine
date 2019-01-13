#ifndef DEMOSCENE5_H
#define DEMOSCENE5_H

#include "../Engine/Scene.h"

class DemoScene5: public Scene {
	public:
		DemoScene5();
		~DemoScene5();

		virtual void initializeScene(World* world, SceneManager* manager);

};

#endif