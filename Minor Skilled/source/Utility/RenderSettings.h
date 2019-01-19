#ifndef RENDERSETTINGS_H
#define RENDERSETTINGS_H

#include <bitset>

class RenderSettings {
	public:
		//render mode options
		static const unsigned int Lighting;
		static const unsigned int Shadows;
		static const unsigned int Bloom;
		static const unsigned int FXAA;
		static const unsigned int MotionBlur;
		static const unsigned int Deferred;
		static const unsigned int SSAO;
		static const unsigned int SSR;
		static const unsigned int PBR;

		static unsigned int Options;

		//face culling options
		static const unsigned int CullNone;
		static const unsigned int CullFront;
		static const unsigned int CullBack;

		static unsigned int CullMode;

		//configurations
		static const unsigned int ShadowWidth;
		static const unsigned int ShadowHeight;

		static bool ShowDirectionalShadows;
		static bool ShowCubeShadows;

		static float CubeShadowNearPlane;
		static float CubeShadowFarPlane;
		static const unsigned int MaxCubeShadows;

		static float DirectionalShadowNearPlane;
		static float DirectionalShadowFarPlane;
		static float DirectionalShadowSize;
		static float DirectionalLightOffset;

		static const float EnvironmentNearPlane;
		static const float EnvironmentFarPlane;

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

		static int SsaoUsedSamples;
		static float SsaoRadius;
		static float SsaoBias;
		static float SsaoPower;

		static float SsrRayStepSize;
		static int SsrMaxRaySteps;
		static float SsrFresnelExponent;
		static float SsrMaxDelta;
		static bool SsrDebug;

		static const unsigned int MaxLights;

		static unsigned int BloomBlurAmount;

		static float Gamma;
		static float Exposure;

		static int MotionBlurSamples;
		static float VelocityScale;

		static float FxaaSpanMax;
		static float FxaaReduceMin;
		static float FxaaReduceMul;

		//helper functions
		static void Enable(unsigned int options);
		static void Disable(unsigned int options);
		static bool IsEnabled(unsigned int options);
};

#endif