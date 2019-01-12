#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Texture;
class Renderbuffer;

class Framebuffer {
	public:
		Framebuffer();
		~Framebuffer();

		unsigned int& getID();

		void bind(GLenum target = GL_FRAMEBUFFER);

		void attachTexture(GLenum attachment, GLenum textureTarget, Texture* texture, GLint mipmapLevel = 0);
		void attachCubemap(GLenum attachment, Texture* cubemap);
		void attachRenderbuffer(GLenum attachment, Renderbuffer* renderbuffer);

		void setDrawBuffers(unsigned int amount, const GLenum* buffers);
		void setReadBuffer(GLenum target);
		void setDrawBuffer(GLenum target);

		void checkForCompletion(std::string fboName);

		static void Unbind(GLenum target = GL_FRAMEBUFFER);

	private:
		unsigned int _id;
};

#endif