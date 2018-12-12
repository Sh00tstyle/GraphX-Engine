#include "OverlayUI.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../../dependencies/imgui/imgui.h"
#include "../../dependencies/imgui/imgui_impl_opengl3.h"
#include "../../dependencies/imgui/imgui_impl_glfw.h"

#include "../Engine/Window.h"

OverlayUI::OverlayUI(Window* window) {
	_initImgui(window);
}

OverlayUI::~OverlayUI() {
	//free resrouces
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void OverlayUI::setupFrame() {
	//start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//setup ui windows
	_setupProfiler();
	_setupConsole();
	_setupHierarchy();
	_setupInspector();
	_setupSettings();
}

void OverlayUI::render() {
	//render the data we set up
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void OverlayUI::_initImgui(Window* window) {
	//initialize imgui for OpenGL in the GLFW window context
	ImGui::CreateContext(); //create imgui context
	ImGui_ImplGlfw_InitForOpenGL(window->getGlfwWindow(), true); //tell imgui to use the glfw window
	ImGui_ImplOpenGL3_Init("#version 460");

	ImGui::StyleColorsDark();

	std::cout << "Initialized ImGui Overlay" << std::endl;
}

void OverlayUI::_setupProfiler() {
	//setup profiler window
	ImGui::Begin("GraphX Engine");
	ImGui::End();
}

void OverlayUI::_setupConsole() {
}

void OverlayUI::_setupHierarchy() {
}

void OverlayUI::_setupInspector() {
}

void OverlayUI::_setupSettings() {
}
