#include "Osc.hpp"

void Osc::Init (float sample_rate) {
		freq_ = 440.0f;
		phase_ = 0.0f;
		phase_inc_ = CalcPhaseInc(freq_);
		sr_ = (1.0f / sample_rate);
}

void Osc::Control(float in) {
	freq_ = (16.352 * 4) * powf(2.0, in * 5);
	phase_inc_ = CalcPhaseInc(freq_);
}

float Osc::Process(float in) {
	float out;
	out = sinf(phase_);
	phase_ += phase_inc_;
	if (phase_ > TWO_PI_F) {
		phase_ -= TWO_PI_F;
	}
	return out;
}
