#ifndef BUFFER_H
#define BUFFER_H

#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../Engine/Vertex.h"

class Buffer {
	public:
		Buffer(GLenum target);
		~Buffer();

		unsigned int& getID();

		void bind();

		void allocateMemory(unsigned int memory);
		void bufferData(const void* data, unsigned int memory);
		void bufferSubData(unsigned int offset, unsigned int memory, const void * data);
		void bindBufferRange(unsigned int index, unsigned int memory);

		static void Unbind(GLenum target);

	private:
		unsigned int _id;
		GLenum _target;
};

#endif