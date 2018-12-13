#ifndef OVERLAY_H
#define OVERLAY_H

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

		bool _renderUI;

		void _initImgui(Window* window);

		void _setupProfiler();
		void _setupConsole();
		void _setupSettings();
		void _setupHierarchy(World* world);
		void _setupInspector();

		void _drawHierarchyNodes(Node* node, unsigned int depth);
};

#endif