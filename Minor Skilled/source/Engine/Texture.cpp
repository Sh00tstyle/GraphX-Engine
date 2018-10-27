#include "Texture.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../Engine/stb_image.h"

#include "../Utility/Filepath.h"

Texture::Texture() {
}

Texture::~Texture() {
}

unsigned int Texture::getID() {
	return _id;
}

Texture * Texture::LoadTexture(std::string path, TextureFilter filter, bool sRGB) {
	//create opengl texture object
	Texture* texture = new Texture();
	glGenTextures(1, &texture->_id);

	//load texture from file
	int width, height, nrComponents;
	unsigned char* textureData = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);

	if(textureData) {
		//identify format
		GLenum internalFormat;
		GLenum dataFormat;

		if(nrComponents == 1) {
			internalFormat = dataFormat = GL_RED;
		} else if(nrComponents == 3) {
			internalFormat = sRGB ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGB;
		} else if(nrComponents == 4) {
			internalFormat = sRGB ? GL_SRGB_ALPHA : GL_RGBA;
			dataFormat = GL_RGBA;
		}

		//load texture into opengl
		glBindTexture(GL_TEXTURE_2D, texture->_id);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, textureData);
		glGenerateMipmap(GL_TEXTURE_2D);

		//set texture filter options
		switch(filter) {
			case TextureFilter::Repeat:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				break;

			case TextureFilter::ClampToEdge:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				break;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(textureData); //free memory

	} else {
		std::cout << "Texture failed to load at path: " + path << std::endl;
		stbi_image_free(textureData); //free memory
	}

	return texture; //texture id
}

Texture * Texture::LoadCubemap(std::vector<std::string> faces) {
	Texture* texture = new Texture();

	glGenTextures(1, &texture->_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture->_id);

	int width, height, nrChannels;
	std::string filename;

	for(unsigned int i = 0; i < faces.size(); i++) {
		filename = Filepath::SkyboxPath + faces[i];

		unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
		if(data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		} else {
			std::cout << "Cubemap texture failed to load at path: " + faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return texture;
}