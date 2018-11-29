#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Renderbuffer {
	public:
		Renderbuffer();
		~Renderbuffer();

		unsigned int& getID();

		void bind();
		void init(GLenum format, unsigned int width, unsigned int height);
		void initMultisample(unsigned int samples, GLenum format, unsigned int width, unsigned int height);

	private:
		unsigned int _id;

		void _generate();
};

#endif