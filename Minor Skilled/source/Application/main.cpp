#include <iostream>
#include <string>

#include "../Application/DemoScene.h"

int main() {
	Scene* scene = new DemoScene();
	scene->initialize();
	scene->run();

	return 0;
}