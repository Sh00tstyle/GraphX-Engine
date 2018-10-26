#ifndef INPUT_H
#define INPUT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../Utility/Key.h"
#include "../Utility/MouseButton.h"

class Input {
	public:
		static void Initialize(GLFWwindow* window);

		static void ProcessInput();

		static bool GetKey(Key key);
		static bool GetKeyDown(Key key);
		static bool GetKeyUp(Key key);

		static bool GetMouse(MouseButton mouseButton);
		static bool GetMouseDown(MouseButton mouseButton);
		static bool GetMouseUp(MouseButton mouseButton);

		static glm::vec2 GetLastMousePos();
		static glm::vec2 GetCurrentMousePos();

	private:
		static glm::vec2 _LastMousePos;
		static GLFWwindow* _Window;

		static void _UpdateMouse();
};

#endif