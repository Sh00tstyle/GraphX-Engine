#ifndef OVERLAY_H
#define OVERLAY_H

class Window;

class OverlayUI {
	public:
		OverlayUI(Window* window);
		~OverlayUI();

		void render();

	private:
		void _initImgui(Window* window);

		void _startFrame();
		void _renderProfiler();
};

#endif