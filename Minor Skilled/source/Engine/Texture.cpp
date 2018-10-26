#include "Texture.h"

Texture::Texture() {
}

Texture::Texture(std::string path, bool sRGB) {
	_loadFromFile(path, sRGB);
}

Texture::~Texture() {
}

unsigned int Texture::getID() {
	return _id;
}

void Texture::_loadFromFile(std::string path, bool sRGB) {
	//TODO
}
