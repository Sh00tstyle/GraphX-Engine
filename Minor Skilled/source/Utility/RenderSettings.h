#ifndef RENDERSETTINGS_H
#define RENDERSETTINGS_H

#include <bitset>

class RenderSettings {
	public:
		//render mode options
		static const unsigned char Shadows;
		static const unsigned char Bloom;
		static const unsigned char Deferred;
		static const unsigned char SSAO;
		static const unsigned char EnvironmentMapping;
		static const unsigned char FXAA;

		static std::bitset<8> Options;

		//configurations
		static const unsigned int MaxCubeShadows;
		static const unsigned int ShadowWidth;
		static const unsigned int ShadowHeight;
		static float CubeNearPlane;
		static float CubeFarPlane;

		static const unsigned int EnvironmentWidth;
		static const unsigned int EnvironmentHeight;

		static const unsigned int IrradianceWidth;
		static const unsigned int IrradianceHeight;

		static unsigned int SsaoKernelSize;
		static float SsaoRadius;
		static float SsaoBias;
		static float SsaoPower;

		static const unsigned int MaxLights;

		static unsigned int BloomBlurAmount;

		static float Gamma;
		static float Exposure;

		static float FxaaSpanMax;
		static float FxaaReduceMin;
		static float FxaaReduceMul;

		//helper functions
		static void Enable(std::bitset<8> options);
		static void Disable(std::bitset<8> options);
		static bool IsEnabled(std::bitset<8> options);
};

#endif