#include "Generator.hpp"
#include <math.h>

constexpr float TWO_PI_F     = (float)M_TWOPI;
constexpr float TWO_PI_RECIP = 1.0f / TWO_PI_F;

float Oscillator::CalcPhaseInc(float f) {
	return (TWO_PI_F * f) * sr_recip_;
}

float Oscillator::Process() {
	float out;
	out = sinf(phase_);

	phase_ += phase_inc_;
	if (phase_ > TWO_PI_F) {
		phase_ -= TWO_PI_F;
	}
	return out;// * amp_;
}
