#ifndef BUFFER_H
#define BUFFER_H

#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../Engine/Vertex.h"

class Buffer {
	public:
		Buffer();
		~Buffer();

		unsigned int& getID();

		void bind(GLenum target);

		void allocateMemory(GLenum target, unsigned int memory);
		void bufferData(GLenum target, const void* data, unsigned int memory);
		void bufferSubData(GLenum target, unsigned int offset, unsigned int memory, const void * data);
		void bindBufferRange(GLenum target, unsigned int index, unsigned int memory);

	private:
		unsigned int _id;

		void _generate();

};

#endif