#ifndef RENDERSETTINGS_H
#define RENDERSETTINGS_H

enum RenderSettings {
	Shadows = 1 << 0,
	Bloom = 1 << 1,
	Deferred = 1 << 2,
	SSAO = 1 << 3,
	FXAA = 1 << 4
};

#endif