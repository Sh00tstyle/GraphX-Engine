#include "Renderbuffer.h"

Renderbuffer::Renderbuffer() {
	glGenRenderbuffers(1, &_id);
}

Renderbuffer::~Renderbuffer() {
	glDeleteRenderbuffers(1, &_id);
}

unsigned int & Renderbuffer::getID() {
	return _id;
}

void Renderbuffer::bind() {
	glBindRenderbuffer(GL_RENDERBUFFER, _id);
}

void Renderbuffer::init(GLenum format, unsigned int width, unsigned int height) {
	glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
}

void Renderbuffer::initMultisample(unsigned int samples, GLenum format, unsigned int width, unsigned int height) {
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, format, width, height);
}

void Renderbuffer::Unbind() {
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}