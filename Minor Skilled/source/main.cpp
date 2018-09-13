// main.cpp : Defines the entry point for the console application.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glad\glad.h> //IMPORTANT: glad needs to the be included BEFORE glfw, throws errors otherwise!!!
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>

#include "shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height); //register a callback function when the window gets resized
void processInput(GLFWwindow* window);

float vertices[] = {
	// positions         // colors
	0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
	-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
	0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
};

/**
//for indexed drawing
float vertices[] = {
	0.5f,  0.5f, 0.0f,  // top right
	0.5f, -0.5f, 0.0f,  // bottom right
	-0.5f, -0.5f, 0.0f,  // bottom left
	-0.5f,  0.5f, 0.0f   // top left
};
unsigned int indices[] = {
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
};
/**/

int main() {
	//initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//create a window object
	GLFWwindow* window = glfwCreateWindow(800, 600, "Minor Skilled Rendering", NULL, NULL);

	if(window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //register the window resize callback function

	//initialize GLAD
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//create shader program
	Shader* shader = new Shader("shader/vertex.vs", "shader/fragment.fs");

	//generate vao
	unsigned int VAO; //vertex array object, stores all configurations done after binding it
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	//generate vbo
	unsigned int VBO; //vertex buffer object, allocates memory for the vertex data
	glGenBuffers(1, &VBO);

	//copy vertices array into an OpenGL buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //bind the vbo to the GL_ARRAY_BUFFER target
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //copy data into the buffers memory

	/**
	//generate ebo
	unsigned int EBO;
	glGenBuffers(1, &EBO); //element buffer object, allows indexed drawing (reusing vertices by utilizing indices)

	//copy indices array into an OpenGL buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	/**/

	//set vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //position attribute
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //color attribute, needs an offset
	glEnableVertexAttribArray(1);

	//unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode

	//render loop
	while(!glfwWindowShouldClose(window)) {
		//input
		processInput(window);

		//rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//activate shader program to render
		shader->use();

		/**
		//update uniform color
		float time = glfwGetTime(); //time since the application started
		float greenValue = (sin(time) / 2.0f) + 0.5f;
		int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor"); //get uniform location from the fragment shader
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f); //stream vec4 color value to uniform in the fragment shader
		/**/

		//render the triangles
		glBindVertexArray(VAO); //we only need to bind the VAO, since it contains all the information about the VBO, EBO etc.
		glDrawArrays(GL_TRIANGLES, 0, 3); //draw the triangles

		glBindVertexArray(0); //unbind vertex array

		//check and call events and swap buffers
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	//cleanup resources
	glfwTerminate();
	delete shader;

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height); //change the viewport everytime the window is resized
}

void processInput(GLFWwindow* window) {
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true); //tell the window to close when pressign escape
}