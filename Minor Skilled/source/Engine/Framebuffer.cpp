#include "Framebuffer.h"

#include <iostream>

#include "../Engine/Texture.h"
#include "../Engine/Renderbuffer.h"

Framebuffer::Framebuffer() {
	glGenFramebuffers(1, &_id);
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

void Framebuffer::attachTexture(GLenum attachment, GLenum textureTarget, Texture* texture, GLint mipmapLevel) {
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textureTarget, texture->getID(), mipmapLevel);
}

void Framebuffer::attachCubemap(GLenum attachment, Texture* cubemap) {
	glFramebufferTexture(GL_FRAMEBUFFER, attachment, cubemap->getID(), 0);
}

void Framebuffer::attachRenderbuffer(GLenum attachment, Renderbuffer* renderbuffer) {
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderbuffer->getID());
}

void Framebuffer::setDrawBuffers(unsigned int amount, const GLenum* buffers) {
	glDrawBuffers(amount, buffers);
}

void Framebuffer::setReadBuffer(GLenum target) {
	glReadBuffer(target);
}

void Framebuffer::setDrawBuffer(GLenum target) {
	glDrawBuffer(target);
}

void Framebuffer::checkForCompletion(std::string fboName) {
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: " + fboName + " framebuffer is incomplete." << std::endl;
	}
}

void Framebuffer::Unbind(GLenum target) {
	glBindFramebuffer(target, 0);
}
