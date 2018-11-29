#include "Buffer.h"

Buffer::Buffer() {
	_generate();
}

Buffer::~Buffer() {
	glDeleteBuffers(1, &_id);
}

unsigned int& Buffer::getID() {
	return _id;
}

void Buffer::bind(GLenum target) {
	glBindBuffer(target, _id);
}

void Buffer::allocateMemory(GLenum target, unsigned int memory) {
	glBufferData(target, memory, NULL, GL_STATIC_DRAW); //allocate memory but buffer nothing
}

void Buffer::bufferData(GLenum target, const void* data, unsigned int memory) {
	glBufferData(target, memory, data, GL_STATIC_DRAW);
}

void Buffer::bufferSubData(GLenum target, unsigned int offset, unsigned int memory, const void* data) {
	glBufferSubData(target, offset, memory, data);
}

void Buffer::bindBufferRange(GLenum target, unsigned int index, unsigned int memory) {
	glBindBufferRange(target, index, _id, 0, memory); //attach buffer to binding point
}

void Buffer::_generate() {
	glGenBuffers(1, &_id);
}