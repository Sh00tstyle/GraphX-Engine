#ifndef RENDERSETTINGS_H
#define RENDERSETTINGS_H

#include <bitset>

class RenderSettings {
	public:
		//render mode options
		static const unsigned char Shadows;
		static const unsigned char Bloom;
		static const unsigned char FXAA;
		static const unsigned char MotionBlur;
		static const unsigned char Deferred;
		static const unsigned char SSAO;
		static const unsigned char PBR;

		static std::bitset<8> Options;

		static bool VSync;

		//configurations
		static const unsigned int MaxCubeShadows;
		static const unsigned int ShadowWidth;
		static const unsigned int ShadowHeight;
		static const float CubeNearPlane;
		static const float CubeFarPlane;

		static const unsigned int SkyboxWidth;
		static const unsigned int SkyboxHeight;

		static const unsigned int EnvironmentWidth;
		static const unsigned int EnvironmentHeight;

		static const unsigned int IrradianceWidth;
		static const unsigned int IrradianceHeight;

		static const unsigned int PrefilterWidth;
		static const unsigned int PrefilterHeight;

		static const unsigned int MaxMipLevels;

		static const unsigned int SsaoKernelSize;

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