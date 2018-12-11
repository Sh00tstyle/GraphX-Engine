#include "OverlayUI.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include "../Engine/Window.h"

OverlayUI::OverlayUI(Window * window) {
	_initImgui(window);
}

OverlayUI::~OverlayUI() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void OverlayUI::render() {
	_startFrame();

	_renderProfiler();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void OverlayUI::_initImgui(Window* window) {
	ImGui::CreateContext(); //create imgui context
	ImGui_ImplGlfw_InitForOpenGL(window->getGlfwWindow(), true); //tell imgui to use the glfw window
	ImGui_ImplOpenGL3_Init("#version 460");

	ImGui::StyleColorsDark();

	std::cout << "Initialized ImGui Overlay" << std::endl;
}

void OverlayUI::_startFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void OverlayUI::_renderProfiler() {
	ImGui::Begin("GraphX Engine");
	ImGui::End();
}
