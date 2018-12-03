#include "Input.h"

#include <iterator>
#include <iostream>
#include <string>

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
		if(RenderSettings::IsEnabled(RenderSettings::Shadows)) {
			RenderSettings::Disable(RenderSettings::Shadows);
			std::cout << "Disabled Shadows" << std::endl;
		} else {
			RenderSettings::Enable(RenderSettings::Shadows);
			std::cout << "Enabled Shadows" << std::endl;
		}
	}

	//bloom
	if(GetKeyDown(Key::KEY2)) {
		if(RenderSettings::IsEnabled(RenderSettings::Bloom)) {
			RenderSettings::Disable(RenderSettings::Bloom);
			std::cout << "Disabled Bloom" << std::endl;
		} else {
			RenderSettings::Enable(RenderSettings::Bloom);
			std::cout << "Enabled Bloom" << std::endl;
		}
	}

	//deferred
	if(GetKeyDown(Key::KEY3)) {
		if(RenderSettings::IsEnabled(RenderSettings::Deferred)) {
			RenderSettings::Disable(RenderSettings::Deferred);
			std::cout << "Disabled Deferred" << std::endl;
		} else {
			RenderSettings::Enable(RenderSettings::Deferred);
			std::cout << "Enabled Deferred" << std::endl;
		}
	}

	//ssao
	if(GetKeyDown(Key::KEY4)) {
		if(RenderSettings::IsEnabled(RenderSettings::SSAO)) {
			RenderSettings::Disable(RenderSettings::SSAO);
			std::cout << "Disabled SSAO" << std::endl;
		} else {
			RenderSettings::Enable(RenderSettings::SSAO);
			std::cout << "Enabled SSAO" << std::endl;
		}
	}
	
	//environment mapping
	if(GetKeyDown(Key::KEY5)) {
		if(RenderSettings::IsEnabled(RenderSettings::EnvironmentMapping)) {
			RenderSettings::Disable(RenderSettings::EnvironmentMapping);
			std::cout << "Disabled Environment Mapping" << std::endl;
		} else {
			RenderSettings::Enable(RenderSettings::EnvironmentMapping);
			std::cout << "Enabled Environment Mapping" << std::endl;
		}
	}

	//fxaa
	if(GetKeyDown(Key::KEY6)) {
		if(RenderSettings::IsEnabled(RenderSettings::FXAA)) {
			RenderSettings::Disable(RenderSettings::FXAA);
			std::cout << "Disabled FXAA" << std::endl;
		} else {
			RenderSettings::Enable(RenderSettings::FXAA);
			std::cout << "Enabled FXAA" << std::endl;
		}
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