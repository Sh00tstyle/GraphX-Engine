#include "VertexArray.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

VertexArray::VertexArray() {
	glGenVertexArrays(1, &_id);
}

VertexArray::~VertexArray() {
	glDeleteVertexArrays(1, &_id);
}

unsigned int& VertexArray::getID() {
	return _id;
}

void VertexArray::bind() {
	glBindVertexArray(_id);
}

void VertexArray::drawArrays(GLenum primitive, unsigned int startIndex, unsigned int vertexCount) {
	glDrawArrays(primitive, startIndex, vertexCount);
}

void VertexArray::drawElements(GLenum primitive, unsigned int indexAmount, GLenum type, const void* indices) {
	glDrawElements(primitive, indexAmount, type, indices);
}

void VertexArray::setAttribute(unsigned int index, unsigned int size, GLenum type, GLsizei stride, const void* pointer) {
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, size, type, GL_FALSE, stride, pointer);
}

void VertexArray::Unbind() {
	glBindVertexArray(0);
}