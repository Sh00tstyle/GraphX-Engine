#include "Converter.h"

#include <iostream>

#include "../Engine/Texture.h"
#include "../Engine/Shader.h"
#include "../Engine/VertexArray.h"
#include "../Engine/Buffer.h"
#include "../Engine/Framebuffer.h"
#include "../Engine/Renderbuffer.h"
#include "../Engine/Window.h"

#include "../Utility/Filepath.h"
#include "../Utility/RenderSettings.h"

const std::vector<float> Converter::_CubeVertices{
	// back face
	-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
	1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
	1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
	1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
	-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
	-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
	// front face
	-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
	1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
	1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
	1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
	-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
	-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
	// left face
	-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
	-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
	-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
	-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
	-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
	-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
	// right face
	1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
	1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
	1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
	1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
	1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
	1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
	// bottom face
	-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
	1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
	1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
	1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
	-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
	-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
	// top face
	-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
	1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
	1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
	1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
	-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
	-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left 
};

Shader* Converter::_ConversionShader = nullptr;

VertexArray* Converter::_CubeVAO = nullptr;
Buffer* Converter::_CubeVBO = nullptr;

Framebuffer* Converter::_ConversionFBO = nullptr;
Renderbuffer* Converter::_ConversionRBO = nullptr;

Texture* Converter::EquirectangularToCubemap(Texture* input) {
	if(_ConversionShader == nullptr) _InitConversionObjects(); //lazy init

	std::cout << "Converting equirectangular texture to cubemap..." << std::endl;

	//create new cubemap texture
	Texture* cubemap = new Texture();
	cubemap->bind(GL_TEXTURE_CUBE_MAP);

	//init each cubemap face
	for(unsigned int i = 0; i < 6; ++i) {
		cubemap->init(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, GL_RGB16F, RenderSettings::SkyboxWidth, RenderSettings::SkyboxHeight, GL_RGB, GL_FLOAT);
	}

	//set texture filter options
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//setup projection and view matrices
	glm::mat4 conversionProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 conversionViews[] = {
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	//convert HDR equirectangular environment map to cubemap equivalent
	_ConversionShader->use();
	_ConversionShader->setMat4("projectionMatrix", conversionProjection);

	//bind equirectangular input texture
	glActiveTexture(GL_TEXTURE0);
	input->bind(GL_TEXTURE_2D);

	//set viewport and bind to conversion framebuffer
	glViewport(0, 0, RenderSettings::SkyboxWidth, RenderSettings::SkyboxHeight);
	_ConversionFBO->bind(GL_FRAMEBUFFER);
	_CubeVAO->bind();

	for(unsigned int i = 0; i < 6; ++i) {
		//attach current face of the cubemap to the fbo
		_ConversionFBO->attachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_ConversionShader->setMat4("viewMatrix", conversionViews[i]);

		//render the cube
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	//bind back to default framebuffer and reset viewport
	glViewport(0, 0, Window::ScreenWidth, Window::ScreenHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);

	delete input; //the input texture is not needed anymore

	//return new cubemap
	return cubemap;
}

void Converter::_InitConversionObjects() {
	//init shader
	_ConversionShader = new Shader(Filepath::ShaderPath + "skybox shader/equiToCube.vs", Filepath::ShaderPath + "skybox shader/equiToCube.fs");

	_ConversionShader->use();
	_ConversionShader->setInt("equirectangularMap", 0);

	//setup cube VAO and VBO
	_CubeVAO = new VertexArray();
	_CubeVAO->bind();

	_CubeVBO = new Buffer();
	_CubeVBO->bind(GL_ARRAY_BUFFER);
	_CubeVBO->bufferData(GL_ARRAY_BUFFER, &_CubeVertices[0], _CubeVertices.size() * sizeof(float));

	glEnableVertexAttribArray(0); //vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1); //normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2); //uv
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	//unbind VAO and VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//init conversion RBO
	_ConversionRBO = new Renderbuffer();
	_ConversionRBO->bind();
	_ConversionRBO->init(GL_DEPTH_COMPONENT24, RenderSettings::SkyboxWidth, RenderSettings::SkyboxHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//init conversion FBO
	_ConversionFBO = new Framebuffer();
	_ConversionFBO->bind(GL_FRAMEBUFFER);
	_ConversionFBO->attachRenderbuffer(GL_DEPTH_ATTACHMENT, _ConversionRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}