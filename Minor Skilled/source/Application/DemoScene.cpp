#include "DemoScene.h"

#include <iostream>
#include <string>

#include "../Engine/Scene.h"

DemoScene::DemoScene():Scene() {
}

DemoScene::~DemoScene() {
}

void DemoScene::_initializeScene() {
	std::cout << "Initialized Scene" << std::endl;
}
