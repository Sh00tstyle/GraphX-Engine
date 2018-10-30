#ifndef RENDERSETTINGS_H
#define RENDERSETTINGS_H

enum RenderSettings {
	Deferred = 1 << 0,
	Bloom = 1 << 1,
	SSAO = 1 << 2,
	FXAA = 1 << 3,
	Shadows = 1 << 4
};

#endif