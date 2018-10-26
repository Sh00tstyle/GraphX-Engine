#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H

#include <glm/glm.hpp>

#include "../Engine/Component.h"

class CameraComponent : public Component {
public:
	CameraComponent();
	~CameraComponent();

	glm::mat4 projectionMatrix;

};

#endif