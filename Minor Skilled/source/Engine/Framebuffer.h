#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Texture;
class Renderbuffer;

class Framebuffer {
	public:
		Framebuffer();
		~Framebuffer();

		unsigned int& getID();

		void bind(GLenum target);

		void attachTexture(GLenum attachment, GLenum textureTarget, Texture* texture, GLint mipmapLevel = 0);
		void attachCubemap(GLenum attachment, Texture* cubemap);
		void attachRenderbuffer(GLenum attachment, Renderbuffer* renderbuffer);

	private:
		unsigned int _id;

		void _generate();
};

#endif