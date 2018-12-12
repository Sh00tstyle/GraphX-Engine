#ifndef OVERLAY_H
#define OVERLAY_H

class Window;

class OverlayUI {
	public:
		OverlayUI(Window* window);
		~OverlayUI();

		void setupFrame();
		void render();

	private:
		void _initImgui(Window* window);

		void _setupProfiler();
		void _setupConsole();
		void _setupSettings();
		void _setupHierarchy();
		void _setupInspector();
};

#endif