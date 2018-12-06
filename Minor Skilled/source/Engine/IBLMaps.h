#ifndef IBLMAPS_H
#define IBLMAPS_H

class Texture;

struct IBLMaps {
	public:
		Texture* environmentMap;
		Texture* irradianceMap;
		Texture* prefilterMap;
		Texture* brdfLUT;
};

#endif