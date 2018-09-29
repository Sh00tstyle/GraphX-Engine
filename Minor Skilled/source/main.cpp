//main.cpp : Defines the entry point for the console application.

#include <iostream>
#include <map>

#include <glad\glad.h> //IMPORTANT: glad needs to the be included BEFORE glfw, throws errors otherwise!!!
#include <GLFW\glfw3.h>

#define STB_IMAGE_IMPLEMENTATION //IMPORTANT: has to be done once in the project BEFORE including std_image.h
#include "stb_image.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Mesh.h"

//functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height); //register a callback function when the window gets resized
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(char const* filepath, bool gammaCorrection);
unsigned int loadCubemap(std::vector<std::string> faces);

void renderQuad();
void renderCube();

//screen settings
unsigned int screenWidth = 1280;
unsigned int screenHeight = 720;
bool useBloom = true;
bool bloomKeyPressed = false;
float exposure = 1.0f;

//time
float deltaTime = 0.0f; //time between the current and the last frame
float lastFrameTime = 0.0f; //time of the last frame

//camera setup
Camera* camera = new Camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastMouseX = screenWidth / 2.0f;
float lastMouseY = screenHeight / 2.0f;
bool firstMouse = true;

//meshes
unsigned int quadVAO = 0;
unsigned int quadVBO;
unsigned int cubeVAO = 0;
unsigned int cubeVBO;

int main() {
	//initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//create a window object
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Minor Skilled Rendering", NULL, NULL);

	if(window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//register callbacks
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //register the window resize callback function															
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//set input mode
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //"captures" the cursor and makes it invisible

	//initialize GLAD
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST); //enable depth test (z-buffer/depth buffer)

	//create shader programs
	Shader* bloomSceneShader = new Shader("assets/shader/bloomScene.vs", "assets/shader/bloomScene.fs");
	Shader* bloomScreenShader = new Shader("assets/shader/bloomScreen.vs", "assets/shader/bloomScreen.fs");
	Shader* blurShader = new Shader("assets/shader/blur.vs", "assets/shader/blur.fs");
	Shader* lightShader = new Shader("assets/shader/bloomScene.vs", "assets/shader/lightBox.fs");

	//load textures
	unsigned int woodTexture = loadTexture("assets/textures/wood.png", true);
	unsigned int containerTexture = loadTexture("assets/textures/container2.png", true);

	//configure floating point framebuffer
	unsigned int hdrFBO;
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

	//create and attach two color buffers
	unsigned int colorBuffers[2];
	glGenTextures(2, colorBuffers);

	for(unsigned int i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL); //assign a 16 bit color buffer
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //clamp to the edge as the blur filter would otherwise sample repeated texture values
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//attach textures to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}

	//create and attach depth renderbuffer
	unsigned int hdrRBO;
	glGenRenderbuffers(1, &hdrRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, hdrRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, hdrRBO);

	//tell OpenGL which attachments we will use in the framebuffer
	unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, attachments);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//ping-pong framebuffers for blurring
	unsigned int pingpongFBO[2];
	unsigned int pingpongBuffers[2];
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongBuffers);

	for(unsigned int i = 0; i < 2; i++) {
		//setup texture and attach to framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //we clamp to the edge as the blur filter would otherwise sample repeated texture values
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffers[i], 0);

		//also check if framebuffers are complete (no need for depth buffer)
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) std::cout << "Framebuffer not complete!" << std::endl;
	}

	//light information
	std::vector<glm::vec3> lightPositions;
	lightPositions.push_back(glm::vec3(0.0f, 0.5f, 1.5f));
	lightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
	lightPositions.push_back(glm::vec3(3.0f, 0.5f, 1.0f));
	lightPositions.push_back(glm::vec3(-.8f, 2.4f, -1.0f));

	std::vector<glm::vec3> lightColors;
	lightColors.push_back(glm::vec3(2.0f, 2.0f, 2.0f));
	lightColors.push_back(glm::vec3(1.5f, 0.0f, 0.0f));
	lightColors.push_back(glm::vec3(0.0f, 0.0f, 1.5f));
	lightColors.push_back(glm::vec3(0.0f, 1.5f, 0.0f));

	//shader configuration
	bloomSceneShader->use();
	bloomSceneShader->setInt("diffuseTexture", 0);

	blurShader->use();
	blurShader->setInt("image", 0);

	bloomScreenShader->use();
	bloomScreenShader->setInt("scene", 0);
	bloomScreenShader->setInt("bloomBlur", 1);

	//render loop
	while(!glfwWindowShouldClose(window)) {
		//update time
		float currentFrameTime = (float)glfwGetTime();
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		//input
		processInput(window);

		//clear relevant buffers
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//setup mvp
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		glm::mat4 viewMatrix = camera->getViewMatrix();
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera->zoom), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);

		//render to floating point framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//set shader uniforms
		bloomSceneShader->use();
		bloomSceneShader->setMat4("viewMatrix", viewMatrix);
		bloomSceneShader->setMat4("projectionMatrix", projectionMatrix);

		for(unsigned int i = 0; i < lightPositions.size(); i++) {
			bloomSceneShader->setVec3("lights[" + std::to_string(i) + "].position", lightPositions[i]);
			bloomSceneShader->setVec3("lights[" + std::to_string(i) + "].color", lightColors[i]);
		}

		bloomSceneShader->setVec3("cameraPos", camera->position);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);

		//create one large cube that acts as the floor
		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -1.0f, 0.0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(12.5f, 0.5f, 12.5f));
		bloomSceneShader->setMat4("modelMatrix", modelMatrix);
		renderCube();

		//then create multiple cubes as the scenery
		glBindTexture(GL_TEXTURE_2D, containerTexture);

		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 1.5f, 0.0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
		bloomSceneShader->setMat4("modelMatrix", modelMatrix);
		renderCube();

		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(2.0f, 0.0f, 1.0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
		bloomSceneShader->setMat4("modelMatrix", modelMatrix);
		renderCube();

		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.0f, -1.0f, 2.0));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		bloomSceneShader->setMat4("modelMatrix", modelMatrix);
		renderCube();

		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 2.7f, 4.0));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.25));
		bloomSceneShader->setMat4("modelMatrix", modelMatrix);
		renderCube();

		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-2.0f, 1.0f, -3.0));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		bloomSceneShader->setMat4("modelMatrix", modelMatrix);
		renderCube();

		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-3.0f, 0.0f, 0.0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
		bloomSceneShader->setMat4("modelMatrix", modelMatrix);
		renderCube();

		// finally show all the light sources as bright cubes
		lightShader->use();
		lightShader->setMat4("viewMatrix", viewMatrix);
		lightShader->setMat4("projectionMatrix", projectionMatrix);

		for(unsigned int i = 0; i < lightPositions.size(); i++) {
			modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(lightPositions[i]));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
			lightShader->setMat4("modelMatrix", modelMatrix);
			lightShader->setVec3("lightColor", lightColors[i]);
			renderCube();
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//blur bright fragments with two-pass Gaussian Blur 
		bool horizontal = true;
		bool firstIteration = true;
		unsigned int amount = 10;

		blurShader->use();

		for(unsigned int i = 0; i < amount; i++) {
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);

			blurShader->setInt("horizontal", horizontal);
			glBindTexture(GL_TEXTURE_2D, firstIteration ? colorBuffers[1] : pingpongBuffers[!horizontal]);  //bind texture of other framebuffer (or scene if first iteration)
			renderQuad();

			horizontal = !horizontal;
			if(firstIteration) firstIteration = false;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//render to screen and tonemap HDR colors from floating point framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		bloomScreenShader->use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffers[!horizontal]);

		bloomScreenShader->setInt("useBloom", useBloom);
		bloomScreenShader->setFloat("exposure", exposure);
		renderQuad();

		//check and call events and swap buffers
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	//cleanup resources
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	//update screen height und width
	screenWidth = width;
	screenHeight = height;
	glViewport(0, 0, width, height); //change the viewport everytime the window is resized
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
	if(firstMouse) {
		//initialize
		lastMouseX = xPos;
		lastMouseY = yPos;
		firstMouse = false;
	}

	//calculate offset movement between the last and the current frame
	float xOffset = xPos - lastMouseX;
	float yOffset = lastMouseY - yPos; //reversed since y-coordinates range from bottom to top

	lastMouseX = xPos;
	lastMouseY = yPos;

	camera->processMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
	camera->processMouseScroll((float)yOffset);
}

void processInput(GLFWwindow* window) {
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true); //tell the window to close when pressign escape

	float cameraSpeed = 2.5f * deltaTime;

	//camera movement input
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera->processKeyboard(FORWARD, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera->processKeyboard(BACKWARD, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera->processKeyboard(LEFT, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera->processKeyboard(RIGHT, deltaTime);

	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !bloomKeyPressed) {
		useBloom = !useBloom;
		bloomKeyPressed = true;
	}
	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
		bloomKeyPressed = false;
	}

	if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		if(exposure > 0.0f) exposure -= 0.001f;
		else exposure = 0.0f;
	} else if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		exposure += 0.001f;
	}

	std::cout << "bloom: " << (useBloom ? "on" : "off") << "| exposure: " << exposure << std::endl;
}

unsigned int loadTexture(char const* filepath, bool gammaCorrection) {
	//create opengl texture object
	unsigned int textureID;
	glGenTextures(1, &textureID);

	//load texture from file
	int width, height, nrComponents;
	unsigned char* textureData = stbi_load(filepath, &width, &height, &nrComponents, 0);

	if(textureData) {
		//identify format
		GLenum internalFormat;
		GLenum dataFormat;

		if(nrComponents == 1) {
			internalFormat = dataFormat = GL_RED;
		} else if(nrComponents == 3) {
			internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGB;
		} else if(nrComponents == 4) {
			internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
			dataFormat = GL_RGBA;
		}

		//load texture into opengl
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, textureData);
		glGenerateMipmap(GL_TEXTURE_2D);

		//set texture filter options
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(textureData); //free memory

	} else {
		std::cout << "Texture failed to load at path: " << filepath << std::endl;
		stbi_image_free(textureData); //free memory
	}

	return textureID; //texture id
}

unsigned int loadCubemap(std::vector<std::string> faces) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	std::string filename;

	for(unsigned int i = 0; i < faces.size(); i++) {
		filename = "assets/skybox/" + faces[i];

		unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
		if(data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
						 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		} else {
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void renderQuad() {
	if(quadVAO == 0) {
		float quadVertices[] = {
			//positions        //uv
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		//setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void renderCube() {
	//lazy initialize
	if(cubeVAO == 0) {
		float vertices[] = {
			//back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, //bottom-left
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, //top-right
			1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, //bottom-right         
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, //top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, //bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, //top-left
			//front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, //bottom-left
			1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, //bottom-right
			1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, //top-right
			1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, //top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, //top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, //bottom-left
			//left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, //top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, //top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, //bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, //bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, //bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, //top-right
			//right face
			1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, //top-left
			1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, //bottom-right
			1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, //top-right         
			1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, //bottom-right
			1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, //top-left
			1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, //bottom-left     
			//bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, //top-right
			1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, //top-left
			1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, //bottom-left
			1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, //bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, //bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, //top-right
			//top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, //top-left
			1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, //bottom-right
			1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, //top-right     
			1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, //bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, //top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  //bottom-left        
		};

		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	//render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}