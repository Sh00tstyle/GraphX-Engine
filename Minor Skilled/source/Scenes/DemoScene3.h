#ifndef DEMOSCENE3_H
#define DEMOSCENE3_H

#include "../Engine/Scene.h"

class DemoScene3: public Scene {
	public:
		DemoScene3();
		~DemoScene3();

		virtual void initializeScene(World* world, SceneManager* manager);

};

#endif