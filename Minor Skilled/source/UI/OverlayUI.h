#ifndef OVERLAY_H
#define OVERLAY_H

#include <string>
#include <sstream>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

class Window;
class Debug;
class World;
class Node;

class OverlayUI {
	public:
		OverlayUI(Window* window, Debug* profiler);
		~OverlayUI();

		void setupFrame(World* world);
		void render();

	private:
		Debug * _profiler;
		Node* _activeNode;

		bool _renderUI;

		std::stringstream _stream;

		void _initImgui(Window* window);

		void _setupProfiler();
		void _setupConsole();
		void _setupSettings();
		void _setupHierarchy(World* world);
		void _setupInspector();

		void _drawHierarchyNodes(Node* node, unsigned int depth);

		std::string _getNameFromPath(std::string path);
};

#endif