#ifndef INPUT_H
#define INPUT_H

#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../Utility/Key.h"
#include "../Utility/MouseButton.h"

class Input {
	public:
		static void Initialize(GLFWwindow* window);

		static void ProcessInput();
		static void ResetMousePos();
		static void CheckInputStatus();

		static bool GetKey(Key key);
		static bool GetKeyDown(Key key);
		static bool GetKeyUp(Key key);

		static bool GetMouse(MouseButton mouseButton);
		static bool GetMouseDown(MouseButton mouseButton);
		static bool GetMouseUp(MouseButton mouseButton);

		static glm::dvec2 GetLastMousePos();
		static glm::dvec2 GetCurrentMousePos();

	private:
		static std::map<Key, bool> _KeysReleased;
		static std::map<MouseButton, bool> _MouseButtonsReleased;

		static GLFWwindow* _Window;

		static bool _FirstMouse;
		static glm::dvec2 _LastMousePos; //movement offset since the last frame
		static glm::dvec2 _CurrentMousePos;

		static void _MouseCallback(GLFWwindow* window, double xPos, double yPos);
};

#endif