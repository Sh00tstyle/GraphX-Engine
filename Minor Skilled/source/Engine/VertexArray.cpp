#include "VertexArray.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

VertexArray::VertexArray() {
	_generate();
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

void VertexArray::_generate() {
	glGenVertexArrays(1, &_id);
}