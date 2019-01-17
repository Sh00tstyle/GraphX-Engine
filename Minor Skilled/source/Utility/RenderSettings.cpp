#include "RenderSettings.h"

//render mode options
const unsigned int RenderSettings::Shadows = 1 << 0;
const unsigned int RenderSettings::Bloom = 1 << 1;
const unsigned int RenderSettings::FXAA = 1 << 2;
const unsigned int RenderSettings::MotionBlur = 1 << 3;
const unsigned int RenderSettings::Deferred = 1 << 4;
const unsigned int RenderSettings::SSAO = 1 << 5; //deferred only
const unsigned int RenderSettings::SSR = 1 << 6; //deferred only
const unsigned int RenderSettings::PBR = 1 << 7; //deferred only

//active render modes
unsigned int RenderSettings::Options = 0;

//face culling options
const unsigned int RenderSettings::CullNone = 0;
const unsigned int RenderSettings::CullFront = 1;
const unsigned int RenderSettings::CullBack = 2;

unsigned int RenderSettings::CullMode = CullNone;

//shadow configurations
const unsigned int RenderSettings::ShadowWidth = 1024;
const unsigned int RenderSettings::ShadowHeight = 1024;

bool RenderSettings::ShowDirectionalShadows = true;
bool RenderSettings::ShowCubeShadows = true;

float RenderSettings::CubeShadowNearPlane = 0.5f;
float RenderSettings::CubeShadowFarPlane = 7.5f;
const unsigned int RenderSettings::MaxCubeShadows = 5;

float RenderSettings::DirectionalShadowNearPlane = 1.0f;
float RenderSettings::DirectionalShadowFarPlane = 10.0f;
float RenderSettings::DirectionalShadowSize = 15.0f;
float RenderSettings::DirectionalLightOffset = 6.0f;

//cubemap render configurations
const float RenderSettings::EnvironmentNearPlane = 0.5f;
const float RenderSettings::EnvironmentFarPlane = 7.5f;

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
const unsigned int RenderSettings::SsaoKernelSize = 64;

float RenderSettings::SsaoRadius = 0.3f;
float RenderSettings::SsaoBias = 0.025f;
float RenderSettings::SsaoPower = 5.0f;

//SSR configurations
float RenderSettings::SsrRayStepSize = 0.1f;
int RenderSettings::SsrMaxRaySteps = 50;
float RenderSettings::SsrFresnelExponent = 5.0f;
float RenderSettings::SsrMaxDelta = 0.03f;

bool RenderSettings::SsrDebug = false;

//lighting configurations
const unsigned int RenderSettings::MaxLights = 16;

//post-processing configurations
unsigned int RenderSettings::BloomBlurAmount = 4;

float RenderSettings::Gamma = 2.2f;
float RenderSettings::Exposure = 1.0f;

int RenderSettings::MotionBlurSamples = 10;
float RenderSettings::VelocityScale = 0.05f;

//FXAA bias configurations
float RenderSettings::FxaaSpanMax = 6.0f;
float RenderSettings::FxaaReduceMin = 1.0f / 128.0f;
float RenderSettings::FxaaReduceMul = 1.0f / 8.0f;

void RenderSettings::Enable(unsigned int options) {
	Options |= options; //enable options
}

void RenderSettings::Disable(unsigned int options) {
	Options &= ~options; //disable options
}

bool RenderSettings::IsEnabled(unsigned int options) {
	return (Options & options) == options; //returns true, if the options is enabled
}
