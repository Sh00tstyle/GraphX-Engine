#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#include "../Engine/System.h"

class CameraSystem : public System {
	public:
		CameraSystem();
		~CameraSystem();

		virtual void update();
};

#endif