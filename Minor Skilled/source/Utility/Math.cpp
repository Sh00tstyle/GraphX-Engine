#include "Math.h"

float Math::Lerp(float a, float b, float f) {
	return a + f * (b - a); //linear interpolation
}
