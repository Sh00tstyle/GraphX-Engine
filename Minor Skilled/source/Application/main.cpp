#include <iostream>
#include <string>

#include "../Scenes/DemoScene1.h"
#include "../Scenes/DemoScene2.h"
#include "../Scenes/DemoScene3.h"
#include "../Scenes/DemoScene4.h"
#include "../Scenes/DemoScene5.h"

#include "../Engine/SceneManager.h"

int main() {
	SceneManager* sceneManager = new SceneManager();

	//add all scenes to the manager
	sceneManager->addScene(new DemoScene1());
	sceneManager->addScene(new DemoScene2());
	sceneManager->addScene(new DemoScene3());
	sceneManager->addScene(new DemoScene4());
	sceneManager->addScene(new DemoScene5());

	//initialize manager with scene 0
	sceneManager->initialize(1);
	sceneManager->run();

	delete sceneManager;

	return 0;
}