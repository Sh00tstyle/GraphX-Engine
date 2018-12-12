#include "OverlayUI.h"

#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../../dependencies/imgui/imgui.h"
#include "../../dependencies/imgui/imgui_impl_opengl3.h"
#include "../../dependencies/imgui/imgui_impl_glfw.h"

#include "../Engine/Window.h"

#include "../Utility/RenderSettings.h"

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
	_setupInspector();
	_setupSettings();
	_setupHierarchy();
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
	ImGui_ImplOpenGL3_Init("#version 460"); //init imgui for opengl with the glsl version 4.6.0

	ImGui::StyleColorsDark();

	std::cout << "Initialized ImGui Overlay" << std::endl;
}

void OverlayUI::_setupProfiler() {
	//setup profiler window
	ImGui::Begin("Profiler", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowPos(ImVec2(Window::ScreenWidth * 1.0f / 3.0f, Window::ScreenHeight * 4.8f / 6.0f));
	ImGui::SetWindowSize(ImVec2(Window::ScreenWidth * 1.0f / 3.0f, Window::ScreenHeight * 1.2f / 6.0f));

	ImGui::Text("Framerate:\t\t\t\t\t%.2f FPS", ImGui::GetIO().Framerate);
	ImGui::Text("Frametime:\t\t\t\t\t%.4f ms", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("Update:   \t\t\t\t\t0.0000 ms");

	ImGui::Text("Rendering:\t\t\t\t\t0.0000 ms");
	ImGui::Text("\tShadow Pass:  \t\t\t0.0000 ms");
	ImGui::Text("\tGeometry Pass:\t\t\t0.0000 ms");
	ImGui::Text("\tLighting Pass:\t\t\t0.0000 ms");
	ImGui::Text("\tSSAO Pass:\t\t\t\t0.0000 ms");
	ImGui::Text("\tPost Processing Pass: \t0.0000 ms");
	ImGui::Text("\tUI Pass:  \t\t\t\t0.0000 ms");

	ImGui::End();
}

void OverlayUI::_setupConsole() {
	//setup console window
	ImGui::Begin("Console", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowPos(ImVec2(0, Window::ScreenHeight * 4.8f / 6.0f));
	ImGui::SetWindowSize(ImVec2(Window::ScreenWidth * 1.0f / 3.0f, Window::ScreenHeight * 1.2f / 6.0f));
	ImGui::End();
}

void OverlayUI::_setupSettings() {
	//setup render settings window
	ImGui::Begin("Render Settings", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowPos(ImVec2(Window::ScreenWidth * 2.0f / 3.0f, Window::ScreenHeight * 4.8f / 6.0f));
	ImGui::SetWindowSize(ImVec2(Window::ScreenWidth * 1.0f / 3.0f, Window::ScreenHeight * 1.2f / 6.0f));

	unsigned int settings = (unsigned int)(RenderSettings::Options.to_ulong());

	ImGui::CheckboxFlags("Shadows", &settings, RenderSettings::Shadows);
	ImGui::CheckboxFlags("Bloom", &settings, RenderSettings::Bloom);
	ImGui::CheckboxFlags("Deferred", &settings, RenderSettings::Deferred);
	ImGui::CheckboxFlags("FXAA", &settings, RenderSettings::FXAA);
	ImGui::CheckboxFlags("SSAO", &settings, RenderSettings::SSAO);
	ImGui::CheckboxFlags("PBR", &settings, RenderSettings::PBR);

	RenderSettings::Options = settings;

	ImGui::End();
}

void OverlayUI::_setupHierarchy() {
	//setup hierarchy window
	ImGui::Begin("Hierarchy", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetWindowSize(ImVec2(Window::ScreenHeight * 1.2f / 6.0f, Window::ScreenHeight * 4.8f / 6.0f));
	ImGui::End();
}

void OverlayUI::_setupInspector() {
	//setup inspector window
	ImGui::Begin("Inspector", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowPos(ImVec2(Window::ScreenWidth - Window::ScreenHeight * 1.2f / 6.0f, 0));
	ImGui::SetWindowSize(ImVec2(Window::ScreenHeight * 1.2f / 6.0f, Window::ScreenHeight * 4.8f / 6.0f));
	ImGui::End();
}