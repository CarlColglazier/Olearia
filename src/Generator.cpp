#include "Generator.hpp"
//#include <math.h>
#include <cmath>

constexpr float TWO_PI_F     = (float) M_TWOPI;
constexpr float TWO_PI_RECIP = 1.0f / TWO_PI_F;

float SinOscillator::CalcPhaseInc(float f) {
	return (TWO_PI_F * f) * sr_recip_;
}

float SinOscillator::Process() {
	float out;
	out = sinf(phase_);

	phase_ += phase_inc_;
	if (phase_ > TWO_PI_F) {
		phase_ -= TWO_PI_F;
	}
	return out;
}

float FmOscillator::Process() {
    osc_one.SetFreq(car_freq_);
    float mod_f = osc_one.Process();
    osc_two.SetFreq(mod_f * mod_index_ + car_freq_);
    return osc_two.Process() * 0.5f;
}