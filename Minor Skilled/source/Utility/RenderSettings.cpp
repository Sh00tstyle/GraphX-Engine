#include "RenderSettings.h"

//render mode options
const unsigned char RenderSettings::Shadows = 1 << 0;
const unsigned char RenderSettings::Bloom = 1 << 1;
const unsigned char RenderSettings::Deferred = 1 << 2;
const unsigned char RenderSettings::SSAO = 1 << 3;
const unsigned char RenderSettings::FXAA = 1 << 4;

//active render modes
std::bitset<8> RenderSettings::Options = /*RenderSettings::Deferred | */RenderSettings::Bloom | RenderSettings::Shadows | 
                                         RenderSettings::SSAO | RenderSettings::FXAA; //everything enabled

//shadow configurations
const unsigned int RenderSettings::MaxCubeShadows = 5; 

const unsigned int RenderSettings::ShadowWidth = 512;
const unsigned int RenderSettings::ShadowHeight = 512;

//cubemap render configurations
float RenderSettings::CubeNearPlane = 0.5f;
float RenderSettings::CubeFarPlane = 50.0f;

//equirectangular to cubemap configurations
const unsigned int RenderSettings::SkyboxHeight = 512;
const unsigned int RenderSettings::SkyboxWidth = 512;

//enviroment cubemap configurations
const unsigned int RenderSettings::EnvironmentHeight = 256;
const unsigned int RenderSettings::EnvironmentWidth = 256;

//irradiance cubemap configurations
const unsigned int RenderSettings::IrradianceHeight = 32;
const unsigned int RenderSettings::IrradianceWidth = 32;

//prefilter cubemap configurations
const unsigned int RenderSettings::PrefilterHeight = 128;
const unsigned int RenderSettings::PrefilterWidth = 128;

const unsigned int RenderSettings::MaxMipLevels = 5;

//SSAO configurations
unsigned int RenderSettings::SsaoKernelSize = 64;
float RenderSettings::SsaoRadius = 0.3f;
float RenderSettings::SsaoBias = 0.025f;
float RenderSettings::SsaoPower = 5.0f;

//lighting configurations
const unsigned int RenderSettings::MaxLights = 16;

//post-processing configurations
unsigned int RenderSettings::BloomBlurAmount = 10;

float RenderSettings::Gamma = 2.2f;
float RenderSettings::Exposure = 1.0f;

//FXAA bias configurations
float RenderSettings::FxaaSpanMax = 6.0f;
float RenderSettings::FxaaReduceMin = 1.0f / 128.0f;
float RenderSettings::FxaaReduceMul = 1.0f / 8.0f;

void RenderSettings::Enable(std::bitset<8> options) {
	Options |= options; //enable options
}

void RenderSettings::Disable(std::bitset<8> options) {
	Options &= ~options; //disable options
}

bool RenderSettings::IsEnabled(std::bitset<8> options) {
	return (Options & options) == options; //returns true, if the options is enabled
}
