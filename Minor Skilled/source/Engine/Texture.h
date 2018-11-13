#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <vector>

#include "../Utility/TextureFilter.h"

class Texture {
public:
	Texture();
	~Texture();

	unsigned int& getID();

	static Texture* LoadTexture(std::string path, TextureFilter filter = TextureFilter::Repeat, bool sRGB = false);
	static Texture* LoadCubemap(std::vector<std::string>& faces, bool sRGB = false);

private:
	unsigned int _id;
};

#endif