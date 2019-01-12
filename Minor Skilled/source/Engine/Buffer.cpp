#include "Buffer.h"

Buffer::Buffer(GLenum target): _target(target) {
	glGenBuffers(1, &_id);
}

Buffer::~Buffer() {
	glDeleteBuffers(1, &_id);
}

unsigned int& Buffer::getID() {
	return _id;
}

void Buffer::bind() {
	glBindBuffer(_target, _id);
}

void Buffer::allocateMemory(unsigned int memory) {
	glBufferData(_target, memory, NULL, GL_STATIC_DRAW); //allocate memory but buffer nothing
}

void Buffer::bufferData(const void* data, unsigned int memory) {
	glBufferData(_target, memory, data, GL_STATIC_DRAW);
}

void Buffer::bufferSubData(unsigned int offset, unsigned int memory, const void* data) {
	glBufferSubData(_target, offset, memory, data);
}

void Buffer::bindBufferRange(unsigned int index, unsigned int memory) {
	glBindBufferRange(_target, index, _id, 0, memory); //attach buffer to binding point
}

void Buffer::Unbind(GLenum target) {
	glBindBuffer(target, 0);
}