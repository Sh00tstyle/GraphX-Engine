#include "OverlayUI.h"

#include <iostream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../../dependencies/imgui/imgui.h"
#include "../../dependencies/imgui/imgui_impl_opengl3.h"
#include "../../dependencies/imgui/imgui_impl_glfw.h"

#include "../Engine/Window.h"
#include "../Engine/Debug.h"
#include "../Engine/World.h"
#include "../Engine/Node.h"

#include "../Utility/RenderSettings.h"
#include "../Utility/Input.h"

OverlayUI::OverlayUI(Window* window, Debug* profiler) : _renderUI(true), _profiler(profiler) {
	_initImgui(window);
}

OverlayUI::~OverlayUI() {
	//free resrouces
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void OverlayUI::setupFrame(World* world) {
	//start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if(Input::GetKeyDown(Key::ENTER)) _renderUI = !_renderUI; //toggle UI

	//setup ui windows
	if(_renderUI) {
		_setupProfiler();
		_setupConsole();
		_setupInspector();
		_setupSettings();
		_setupHierarchy(world);
	}
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
	ImGui::SetWindowPos(ImVec2(Window::ScreenWidth * 2.0f / 3.0f, Window::ScreenHeight * 4.8f / 6.0f));
	ImGui::SetWindowSize(ImVec2(Window::ScreenWidth * 1.0f / 3.0f, Window::ScreenHeight * 1.2f / 6.0f));

	//Info
	const char* glInfos = (const char*)glGetString(GL_VERSION);
	const char* hardwareInfos = (const char*)glGetString(GL_RENDERER);

	ImGui::Text("OpenGL Version:");
	ImGui::Text(glInfos);
	ImGui::Text("Hardware:");
	ImGui::Text(hardwareInfos);

	ImGui::Text("\nFramerate:  \t\t\t\t\t\t%.2f FPS", ImGui::GetIO().Framerate);
	ImGui::Text("Frametime:  \t\t\t\t\t\t%.4f ms", 1000.0f / ImGui::GetIO().Framerate);

	//CPU
	ImGui::Text("Graph Update:   \t\t\t\t\t%.4f ms", _profiler->getQuery(QueryType::Update));

	//GPU
	ImGui::Text("Rendering:  \t\t\t\t\t\t%.4f ms", _profiler->getQuery(QueryType::Rendering));

	ImGui::Indent();

	if(ImGui::TreeNode("Details:")) {
		if(RenderSettings::IsEnabled(RenderSettings::Shadows)) {
			ImGui::Text("Shadow Pass:  \t\t\t\t%.4f ms", _profiler->getQuery(QueryType::Shadow));
		} else {
			ImGui::Text("Shadow Pass:  \t\t\t\t0.0000 ms");
		}

		if(RenderSettings::IsEnabled(RenderSettings::Deferred)) {
			ImGui::Text("Geometry Pass:\t\t\t\t%.4f ms", _profiler->getQuery(QueryType::Geometry));
			ImGui::Text("Lighting Pass:\t\t\t\t%.4f ms", _profiler->getQuery(QueryType::Lighting));

			if(RenderSettings::IsEnabled(RenderSettings::SSAO)) {
				ImGui::Text("SSAO Pass:\t\t\t\t%.4f ms", _profiler->getQuery(QueryType::SSAO));
			} else {
				ImGui::Text("SSAO Pass:\t\t\t\t0.0000 ms");
			}
		} else {
			ImGui::Text("Geometry Pass:\t\t\t\t0.0000 ms");
			ImGui::Text("Lighting Pass:\t\t\t\t0.0000 ms");
			ImGui::Text("SSAO Pass:\t\t\t\t\t0.0000 ms");
		}

		ImGui::Text("Post Processing Pass: \t\t%.4f ms", _profiler->getQuery(QueryType::PostProcessing));
		ImGui::Text("UI Pass:  \t\t\t\t\t%.4f ms", _profiler->getQuery(QueryType::UI));

		ImGui::TreePop();
	}

	ImGui::End();
}

void OverlayUI::_setupConsole() {
	//setup console window
	ImGui::Begin("Console", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowPos(ImVec2(0, Window::ScreenHeight * 4.8f / 6.0f));
	ImGui::SetWindowSize(ImVec2(Window::ScreenWidth * 1.0f / 3.0f, Window::ScreenHeight * 1.2f / 6.0f));

	//post all messages from debug
	std::vector<std::string> logMessages = Debug::GetLogs();

	for(unsigned int i = 0; i < logMessages.size(); i++) {
		ImGui::Text(logMessages[i].c_str());
	}

	ImGui::End();
}

void OverlayUI::_setupSettings() {
	//setup render settings window
	ImGui::Begin("Render Settings", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowPos(ImVec2(Window::ScreenWidth * 1.0f / 3.0f, Window::ScreenHeight * 4.8f / 6.0f));
	ImGui::SetWindowSize(ImVec2(Window::ScreenWidth * 1.0f / 3.0f, Window::ScreenHeight * 1.2f / 6.0f));

	unsigned int settings = (unsigned int)(RenderSettings::Options.to_ulong()); //optain settings and convert to uint
	int bloomBlur = (int)RenderSettings::BloomBlurAmount;

	ImGui::Text("General");
	ImGui::CheckboxFlags("Shadows", &settings, RenderSettings::Shadows);
	ImGui::CheckboxFlags("Bloom", &settings, RenderSettings::Bloom);
	ImGui::CheckboxFlags("Deferred", &settings, RenderSettings::Deferred);
	ImGui::CheckboxFlags("FXAA", &settings, RenderSettings::FXAA);
	ImGui::CheckboxFlags("Motion Blur", &settings, RenderSettings::MotionBlur);
	ImGui::CheckboxFlags("SSAO", &settings, RenderSettings::SSAO);
	ImGui::CheckboxFlags("PBR", &settings, RenderSettings::PBR);
	ImGui::Checkbox("vSync", &RenderSettings::VSync);

	ImGui::Text("\nPost Processing Settings");
	ImGui::InputFloat("Gamma", &RenderSettings::Gamma);
	ImGui::InputFloat("Exposure", &RenderSettings::Exposure);
	ImGui::InputInt("Bloom Blur", &bloomBlur);

	ImGui::Text("\nSSAO Settings");
	ImGui::InputFloat("Radius", &RenderSettings::SsaoRadius);
	ImGui::InputFloat("Bias", &RenderSettings::SsaoBias);
	ImGui::InputFloat("Power", &RenderSettings::SsaoPower);

	ImGui::Text("\nFXAA Settings");
	ImGui::InputFloat("Max Spam", &RenderSettings::FxaaSpanMax);
	ImGui::InputFloat("Min Reduce", &RenderSettings::FxaaReduceMin);
	ImGui::InputFloat("Mul Reduce", &RenderSettings::FxaaReduceMul);

	//apply settings
	RenderSettings::Options = settings; 
	RenderSettings::BloomBlurAmount = bloomBlur;

	ImGui::End();
}

void OverlayUI::_setupHierarchy(World* world) {
	//setup hierarchy window
	ImGui::Begin("Hierarchy", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetWindowSize(ImVec2(Window::ScreenWidth * 1.0f / 6.0f, Window::ScreenHeight * 4.8f / 6.0f));

	//draw all children and their children onto the UI
	Node* currentNode;

	for(unsigned int i = 0; i < world->getChildCount(); i++) {
		currentNode = world->getChildAt(i);

		_drawHierarchyNodes(currentNode, 0); //current depth is 0
	}

	ImGui::End();
}

void OverlayUI::_setupInspector() {
	//setup inspector window
	ImGui::Begin("Inspector", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowPos(ImVec2(Window::ScreenWidth * 5.0f / 6.0f, 0));
	ImGui::SetWindowSize(ImVec2(Window::ScreenWidth * 1.0f / 6.0f, Window::ScreenHeight * 4.8f / 6.0f));

	//TODO: Components and Materials

	ImGui::End();
}

void OverlayUI::_drawHierarchyNodes(Node* node, unsigned int depth) {
	//add intendions based on the tree depth
	std::string name = "";

	for(unsigned int i = 0; i < depth; i++) {
		name += "\t";
	}

	name += node->getName();

	if(ImGui::Button(name.c_str())) {
		//TODO: Select in inspector
	}

	ImGui::AlignTextToFramePadding();

	//draw child nodes as well recursively
	Node* currentNode;

	for(unsigned int i = 0; i < node->getChildCount(); i++) {
		currentNode = node->getChildAt(i);

		_drawHierarchyNodes(currentNode, depth + 1);
	}
}
