#include "CameraSystem.h"

#include "../Utility/ComponentType.h"

CameraSystem::CameraSystem():System(ComponentType::Transform | ComponentType::Camera) {
}

CameraSystem::~CameraSystem() {
}

void CameraSystem::update() {
}