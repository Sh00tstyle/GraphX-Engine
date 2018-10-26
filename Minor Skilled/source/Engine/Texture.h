#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

class Texture {
public:
	Texture();
	Texture(std::string path, bool sRGB = false);
	~Texture();

	unsigned int getID();

private:
	unsigned int _id;

	void _loadFromFile(std::string path, bool sRGB = false);
};

#endif