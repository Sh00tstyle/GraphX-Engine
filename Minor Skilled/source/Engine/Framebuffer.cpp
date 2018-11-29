#include "Framebuffer.h"

#include "../Engine/Texture.h"
#include "../Engine/Renderbuffer.h"

Framebuffer::Framebuffer() {
	_generate();
}

Framebuffer::~Framebuffer() {
	glDeleteFramebuffers(1, &_id);
}

unsigned int & Framebuffer::getID() {
	return _id;
}

void Framebuffer::bind(GLenum target) {
	glBindFramebuffer(target, _id);
}

void Framebuffer::attachTexture(GLenum attachment, GLenum textureTarget, Texture* texture) {
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textureTarget, texture->getID(), 0);
}

void Framebuffer::attachRenderbuffer(GLenum attachment, Renderbuffer* renderbuffer) {
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderbuffer->getID());
}

void Framebuffer::_generate() {
	glGenFramebuffers(1, &_id);
}
