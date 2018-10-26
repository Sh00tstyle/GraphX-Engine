#include "Time.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

float Time::DeltaTime = 0.0f;
float Time::CurrentTime = 0.0f;
float Time::LastTime = 0.0f;

void Time::UpdateTime() {
	CurrentTime = (float)glfwGetTime();
	DeltaTime = CurrentTime - LastTime;
	LastTime = CurrentTime;
}