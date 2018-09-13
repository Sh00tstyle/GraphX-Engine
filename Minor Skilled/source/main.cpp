// main.cpp : Defines the entry point for the console application.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glad\glad.h> //IMPORTANT: glad needs to the be included BEFORE glfw, throws errors otherwise!!!
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height); //register a callback function when the window gets resized
void processInput(GLFWwindow* window);

std::string readShaderFile(std::string filePath);
void compileShader (std::string shaderSource, unsigned int &shader);
void attachShaders(unsigned int &vertexShader, unsigned int &fragmentShader, unsigned int &shaderProgram);

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

int main()
{
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

	//load shader from file
	std::string vertexShaderSource = readShaderFile("shader/vertex.vs");
	std::cout << vertexShaderSource << std::endl << std::endl;

	std::string fragmentShaderSource = readShaderFile("shader/fragment.fs");
	std::cout << fragmentShaderSource << std::endl << std::endl;

	//create and compile shaders
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	compileShader(vertexShaderSource, vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	compileShader(fragmentShaderSource, fragmentShader);

	//create shader program
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	attachShaders(vertexShader, fragmentShader, shaderProgram);

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

	//generate ebo
	unsigned int EBO;
	glGenBuffers(1, &EBO); //element buffer object, allows indexed drawing (reusing vertices by utilizing indices)

	//copy indices array into an OpenGL buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//set vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode

	//render loop
	while(!glfwWindowShouldClose(window)) {
		//input
		processInput(window);

		//rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//use the shader program to render
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO); //we only need to bind the VAO, since it contains all the information about the VBO, EBO etc.
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //draw the triangles

		glBindVertexArray(0); //unbind vertex array

		//check and call events and swap buffers
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate(); //cleanup resources
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height); //change the viewport everytime the window is resized
}

void processInput(GLFWwindow* window) {
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true); //tell the window to close when pressign escape
}

std::string readShaderFile(std::string filePath) {
	//load shader from file
	std::ifstream shaderFile(filePath);

	//error handling
	if(!shaderFile) {
		std::cout << "Could not read shader file at " + filePath << std::endl;
	}

	//read file into a stream
	std::stringstream buffer;
	buffer << shaderFile.rdbuf();
	shaderFile.close();

    //return data as string
	return buffer.str();
}

void compileShader(std::string shaderSource, unsigned int &shader) {
	//compile shader
	char const* sourcePointer = shaderSource.c_str();
	glShaderSource(shader, 1, &sourcePointer, NULL);
	glCompileShader(shader);

	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if(!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "Shader compilation failed\n" << infoLog << std::endl;
	} else {
		std::cout << "Compiled shader successfully\n" << std::endl;
	}
}

void attachShaders(unsigned int &vertexShader, unsigned int &fragmentShader, unsigned int &shaderProgram) {
	//attach shaders and link them
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	//handle errors
	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "Shader program linking failed\n" << infoLog << std::endl;
	} else {
		std::cout << "Linked shaders successfully\n" << std::endl;
	}

	//delete the shader objects, they are not needed anymore
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}