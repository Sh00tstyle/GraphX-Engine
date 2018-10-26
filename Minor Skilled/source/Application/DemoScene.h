#ifndef DEMOSCENE_H
#define DEMOSCENE_H

#include "../Engine/Scene.h"

class DemoScene: public Scene {
	public:
		DemoScene();
		~DemoScene();

	private:
		virtual void _initializeScene();

};

#endif