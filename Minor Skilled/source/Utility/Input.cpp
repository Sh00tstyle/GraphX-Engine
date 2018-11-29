#include "Input.h"

#include <iterator>
#include <iostream>
#include <string>

#include "../Engine/Renderer.h"

#include "../Utility/RenderSettings.h"

std::map<Key, bool> Input::_KeysReleased;
std::map<MouseButton, bool> Input::_MouseButtonsReleased;
GLFWwindow* Input::_Window = nullptr;
glm::vec2 Input::_LastMousePos = glm::vec2(0.0f, 0.0f);
glm::vec2 Input::_CurrentMousePos = glm::vec2(0.0f, 0.0f);
bool Input::_FirstMouse = true;

void Input::Initialize(GLFWwindow* window) {
	_Window = window;

	//register mouse callbacks
	glfwSetCursorPosCallback(window, _MouseCallback);

	//set input mode
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //captures the cursor and makes it invisible
}

void Input::ProcessInput() {
	if(GetKey(Key::ESC)) glfwSetWindowShouldClose(_Window, true);

	//shadows
	if(GetKeyDown(Key::KEY1)) {
		if(Renderer::IsEnabled(RenderSettings::Shadows)) Renderer::Disable(RenderSettings::Shadows);
		else Renderer::Enable(RenderSettings::Shadows);
	}

	//bloom
	if(GetKeyDown(Key::KEY2)) {
		if(Renderer::IsEnabled(RenderSettings::Bloom)) Renderer::Disable(RenderSettings::Bloom);
		else Renderer::Enable(RenderSettings::Bloom);
	}

	//deferred
	if(GetKeyDown(Key::KEY3)) {
		if(Renderer::IsEnabled(RenderSettings::Deferred)) Renderer::Disable(RenderSettings::Deferred);
		else Renderer::Enable(RenderSettings::Deferred);
	}

	//ssao
	if(GetKeyDown(Key::KEY4)) {
		if(Renderer::IsEnabled(RenderSettings::SSAO)) Renderer::Disable(RenderSettings::SSAO);
		else Renderer::Enable(RenderSettings::SSAO);
	}
}

void Input::ResetMousePos() {
	_LastMousePos = _CurrentMousePos;
}

void Input::CheckInputStatus() {
	//reset the key if needed, done at the end of processing input
	for(std::map<Key, bool>::iterator it = _KeysReleased.begin(); it != _KeysReleased.end(); it++) {
		Key key = it->first;

		if(!GetKey(key) && !_KeysReleased[key]) _KeysReleased[key] = true; //reset key to released state
		else if(GetKey(key) && !_KeysReleased[key]) _KeysReleased[key] = false; //reset key to pressed state
	}

	//reset the mouse button if needed
	for(std::map<MouseButton, bool>::iterator it = _MouseButtonsReleased.begin(); it != _MouseButtonsReleased.end(); it++) {
		MouseButton mouseButton = it->first;

		if(!GetMouse(mouseButton) && !_MouseButtonsReleased[mouseButton]) _MouseButtonsReleased[mouseButton] = true; //reset button to released state
		else if(GetMouse(mouseButton) && !_MouseButtonsReleased[mouseButton]) _MouseButtonsReleased[mouseButton] = false; //reset button to pressed state
	}
}

bool Input::GetKey(Key key) {
	return glfwGetKey(_Window, key);
}

bool Input::GetKeyDown(Key key) {
	//lazy init
	if(!_KeysReleased.count(key)) _KeysReleased[key] = true;

	if(GetKey(key) && _KeysReleased[key]) {
		//update to show that the key is pressed
		_KeysReleased[key] = false;
		return true;
	} 

	return false;
}

bool Input::GetKeyUp(Key key) {
	//lazy init
	if(!_KeysReleased.count(key)) _KeysReleased[key] = true;

	if(!GetKey(key) && !_KeysReleased[key]) { //may skip one frame of detecting a key release in the early frames
		//update to show that the key is released
		_KeysReleased[key] = true;
		return true;
	}

	return false;
}

bool Input::GetMouse(MouseButton mouseButton) {
	return glfwGetMouseButton(_Window, mouseButton);
}

bool Input::GetMouseDown(MouseButton mouseButton) {
	//lazy init
	if(!_MouseButtonsReleased.count(mouseButton)) _MouseButtonsReleased[mouseButton] = true;

	if(GetMouse(mouseButton) && _MouseButtonsReleased[mouseButton]) {
		//update to show that the button is pressed
		_MouseButtonsReleased[mouseButton] = false;
		return true;
	}

	return false;
}

bool Input::GetMouseUp(MouseButton mouseButton) {
	//lazy init
	if(!_MouseButtonsReleased.count(mouseButton)) _MouseButtonsReleased[mouseButton] = true;

	if(!GetMouse(mouseButton) && !_MouseButtonsReleased[mouseButton]) { //may skip one frame of detecting a key release in the early frames
		//update to show that the button is released
		_MouseButtonsReleased[mouseButton] = true;
		return true;
	}

	return false;
}

glm::vec2 Input::GetLastMousePos() {
	return _LastMousePos;
}

glm::vec2 Input::GetCurrentMousePos() {
	return _CurrentMousePos;
}

void Input::_MouseCallback(GLFWwindow * window, double xPos, double yPos) {
	if(_FirstMouse) {
		//initialize
		_CurrentMousePos.x = (float)xPos;
		_CurrentMousePos.y = (float)yPos;
		_FirstMouse = false;
	}

	_LastMousePos = _CurrentMousePos;

	_CurrentMousePos.x = (float)xPos;
	_CurrentMousePos.y = (float)yPos;
}