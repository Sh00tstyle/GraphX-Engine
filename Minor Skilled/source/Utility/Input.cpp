#include "Input.h"

#include <iostream>
#include <string>

GLFWwindow* Input::_Window = nullptr;
glm::vec2 Input::_LastMousePos = glm::vec2(0.0f, 0.0f);
glm::vec2 Input::_CurrentMousePos = glm::vec2(0.0f, 0.0f);
bool Input::_FirstMouse = true;

void Input::Initialize(GLFWwindow * window) {
	_Window = window;

	//register mouse callbacks
	glfwSetCursorPosCallback(window, _MouseCallback);

	//set input mode
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //captures the cursor and makes it invisible
}

void Input::ProcessInput() {
	if(GetKey(Key::ESC)) glfwSetWindowShouldClose(_Window, true);
}

void Input::ResetMousePos() {
	_LastMousePos = _CurrentMousePos;
}

bool Input::GetKey(Key key) {
	return glfwGetKey(_Window, key);
}

bool Input::GetKeyDown(Key key) {
	//TODO
	return false;
}

bool Input::GetKeyUp(Key key) {
	//TODO
	return false;
}

bool Input::GetMouse(MouseButton mouseButton) {
	return glfwGetMouseButton(_Window, mouseButton);
}

bool Input::GetMouseDown(MouseButton mouseButton) {
	//TODO
	return false;
}

bool Input::GetMouseUp(MouseButton mouseButton) {
	//TODO
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