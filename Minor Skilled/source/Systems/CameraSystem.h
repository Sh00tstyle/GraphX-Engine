#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#include "../Engine/System.h"

class TransformComponent;
class CameraComponent;

class CameraSystem : public System {
	public:
		CameraSystem();
		~CameraSystem();

		virtual void update();

	private:
		void _processInput(TransformComponent* transformComponent, CameraComponent* cameraComponent, glm::vec2 mouseOffset);

};

#endif