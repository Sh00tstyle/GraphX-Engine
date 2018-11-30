#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

class Window {
	public:
		Window(unsigned int width, unsigned int height, std::string name);
		~Window();

		static unsigned int ScreenWidth;
		static unsigned int ScreenHeight;

		bool isOpen();
		void close();
		void swapBuffers();
		void pollEvents();

	private:
		GLFWwindow* _glfwWindow;

		static void _framebufferSizeCallback(GLFWwindow* window, int width, int height); //needs to be static to be passed into GLFW

		void _initializeGLFW(int msaa);
		void _initializeWindow(std::string name);
		void _initializeGLAD();
};

#endif