#ifndef GENERATOR_
#define GENERATOR_
#include "Generator.hpp"
#endif

#include <random>

class Noise: public Generator {
public:
	void Init(float sample_rate);
	void Control(float in);
	float Process(float in);
private:
	float amp_;
};
