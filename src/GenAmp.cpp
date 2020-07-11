#include "GenAmp.hpp"

void Amp::Init(float sample_rate) {
	// do nothing here.
}

void Amp::Control(float in) {
	amp_ = in;
}

float Amp::Process(float in) {
	return in * amp_;
}
