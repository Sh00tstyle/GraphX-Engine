#include "RenderSystem.h"

#include "../Utility/ComponentType.h"

RenderSystem::RenderSystem():System(ComponentType::Transform | ComponentType::Render) {
}

RenderSystem::~RenderSystem() {
}

void RenderSystem::render() {
}
