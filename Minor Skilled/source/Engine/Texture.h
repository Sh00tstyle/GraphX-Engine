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
		Texture(GLenum target, GLenum internalFormat, unsigned int width, unsigned int height, GLenum format, GLenum type, GLenum minFilter, GLenum magFilter, GLenum wrap, const void* pixels, bool genMipmaps);
		Texture(GLenum target);
		~Texture();

		std::string filepath;

		unsigned int& getID();

		void bind();

		void generateMipmaps();

		void init(GLenum internalFormat, unsigned int width, unsigned int height, GLenum format, GLenum type, const void* pixels);
		void initTarget(GLenum target, GLenum internalFormat, unsigned int width, unsigned int height, GLenum format, GLenum type, const void* pixels);
		void filter(GLenum minFilter, GLenum magFilter, GLenum wrap);

		static Texture* LoadTexture(std::string path, TextureFilter filter = TextureFilter::Repeat, bool sRGB = false);
		static Texture* LoadCubemap(std::vector<std::string>& faces, bool sRGB = false);
		static Texture* LoadHDR(std::string path);

		static void Unbind(GLenum target);
		static void SetActiveUnit(unsigned int unit);

	private:
		unsigned int _id;
		GLenum _target;
};

#endif