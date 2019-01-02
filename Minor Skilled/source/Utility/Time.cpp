#include "Time.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

double Time::DeltaTime = 0.0f;
double Time::CurrentTime = 0.0f;
double Time::LastTime = 0.0f;

void Time::UpdateTime() {
	CurrentTime = glfwGetTime();
	DeltaTime = CurrentTime - LastTime;
	LastTime = CurrentTime;
}
