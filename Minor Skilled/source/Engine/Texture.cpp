#include "Texture.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../Engine/stb_image.h"

#include "../Utility/Filepath.h"

Texture::Texture() {
	_generate();
}

Texture::~Texture() {
	glDeleteTextures(1, &_id);
}

unsigned int& Texture::getID() {
	return _id;
}

void Texture::bind(GLenum target) {
	glBindTexture(target, _id);
}

void Texture::init(GLenum target, GLenum internalFormat, unsigned int width, unsigned int height, GLenum format, GLenum type) {
	glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, NULL);
}

void Texture::initMultisample(GLenum target, unsigned int samples, GLenum format, unsigned int width, unsigned int height) {
	glTexImage2DMultisample(target, samples, format, width, height, GL_TRUE);
}

Texture * Texture::LoadTexture(std::string path, TextureFilter filter, bool sRGB) { 
	//sRGB textures are essentially gamma corrected already and usually the colorspace they are created in
	//when setting the sRGB parameter to true, OpenGL transform the texture from gamma corrected/sRGB color space back to linear color space so that they can/have to be gamma corrected in the shaders
	//diffuse and color textures are almost always in sRGB space - specular map, normals maps, etc. are almost always in linear space

	//create opengl texture object
	Texture* texture = new Texture();

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
		texture->bind(GL_TEXTURE_2D);
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

Texture * Texture::LoadCubemap(std::vector<std::string>& faces, bool sRGB) {
	Texture* texture = new Texture();
	texture->bind(GL_TEXTURE_CUBE_MAP);

	GLenum internalFormat = sRGB ? GL_SRGB : GL_RGB;
	int width, height, nrChannels;
	std::string filename;

	for(unsigned int i = 0; i < faces.size(); i++) {
		filename = Filepath::SkyboxPath + faces[i];

		unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
		if(data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
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

Texture* Texture::LoadHDR(std::string path) {
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float *data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);

	if(data) {
		Texture* texture = new Texture();
		texture->bind(GL_TEXTURE_2D);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); //load hdr (floating point)

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);

		return texture;
	} else {
		return nullptr;
		std::cout << "Failed to load HDR image." << std::endl;
	}
}

void Texture::_generate() {
	glGenTextures(1, &_id);
}