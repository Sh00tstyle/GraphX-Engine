#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../Utility/TextureFilter.h"

class Shader;
class VertexArray;
class Buffer;
class Framebuffer;

class Texture {
	public:
		Texture();
		~Texture();

		unsigned int& getID();

		void bind(GLenum target);

		void init(GLenum target, GLenum internalFormat, unsigned int width, unsigned int height, GLenum format, GLenum type);
		void initMultisample(GLenum target, unsigned int samples, GLenum format, unsigned int width, unsigned int height);

		static Texture* LoadTexture(std::string path, TextureFilter filter = TextureFilter::Repeat, bool sRGB = false);
		static Texture* LoadCubemap(std::vector<std::string>& faces, bool sRGB = false);
		static Texture* LoadHDR(std::string path);

	private:
		unsigned int _id;

		void _generate();
};

#endif