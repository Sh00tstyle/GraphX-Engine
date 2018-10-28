#include "Window.h"



#include <iostream>

#include "../Utility/Input.h"

unsigned int Window::ScreenWidth = 1280;
unsigned int Window::ScreenHeight = 720;

Window::Window(unsigned int width, unsigned int height, std::string name, unsigned int msaa) {
	ScreenWidth = width;
	ScreenHeight = height;

	_initializeGLFW(msaa);
	_initializeWindow(name);
	_initializeGLAD();

	Input::Initialize(_glfwWindow);
}

Window::~Window() {
	glfwTerminate();
}

bool Window::isOpen() {
	return !glfwWindowShouldClose(_glfwWindow); //returns true as long as the window is open
}

void Window::close() {
	glfwSetWindowShouldClose(_glfwWindow, true);
}

void Window::swapBuffers() {
	glfwSwapBuffers(_glfwWindow);
}

void Window::_initializeGLFW(unsigned int msaa) {
	int success = glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, msaa); //enable MSAA based on parameter (default is 4)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	std::cout << "GLFW Initialization Status: " + std::to_string(success) << std::endl;
}

void Window::_initializeWindow(std::string name) {
	_glfwWindow = glfwCreateWindow(ScreenWidth, ScreenHeight, name.c_str(), NULL, NULL);

	if(_glfwWindow == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	} else {
		std::cout << "Initialized Window" << std::endl;
	}

	glfwMakeContextCurrent(_glfwWindow);
	glfwSetFramebufferSizeCallback(_glfwWindow, _framebufferSizeCallback);
}

void Window::_initializeGLAD() {
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	} else {
		std::cout << "Initialized GLAD" << std::endl;
	}

	glEnable(GL_DEPTH_TEST); //enable the z-buffer
	glEnable(GL_BLEND); //enable blending
	glDepthFunc(GL_LEQUAL); //set depth funtion to lass than AND equal for skybox depth trick
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); //enable seamless cubemap sampling for lower mip map levels in the pre filter map
}

void Window::_framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	ScreenWidth = width;
	ScreenHeight = height;

	//TODO: update the projection matrix

	glViewport(0, 0, width, height);
}
