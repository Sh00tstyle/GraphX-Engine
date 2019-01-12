#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class VertexArray {
	public: 
		VertexArray();
		~VertexArray();

		unsigned int& getID();

		void bind();

		void drawArrays(GLenum primitive, unsigned int startIndex, unsigned int vertexCount);
		void drawElements(GLenum primitive, unsigned int indexAmount, GLenum type, const void* indices);

		void setAttribute(unsigned int index, unsigned int size, GLenum type, GLsizei stride, const void* pointer);

		static void Unbind();

	private:
		unsigned int _id;
};

#endif