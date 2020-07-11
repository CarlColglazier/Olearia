#include "GenNoise.hpp"

void Noise::Init(float sample_rate) {
	amp_ = 1.0;
}

void Noise::Control(float in) {
	amp_ = in;
}

float Noise::Process(float in) {
	return (rand() / (RAND_MAX + 1.)) * amp_;
}
