#include "Input.h"

glm::vec2 Input::_LastMousePos = glm::vec2(0.0f, 0.0f);
GLFWwindow* Input::_Window = nullptr;

void Input::Initialize(GLFWwindow * window) {
	_Window = window;
}

void Input::ProcessInput() {
	_UpdateMouse();
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
	//TODO
	return glm::vec2();
}

void Input::_UpdateMouse() {
	//TODO
}
