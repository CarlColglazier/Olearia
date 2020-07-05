#ifndef GENERATOR_
#define GENERATOR_
#include "Generator.hpp"
#endif

#ifndef OSC_H
#define OSC_H

#include <math.h>

constexpr float TWO_PI_F     = (float)M_TWOPI;

// just a sine wave for now.
// a lot of the design here is taken from DaisySP
class Osc: public Generator {
public:
	void Init(float sample_rate);
	void Control(float in);
	float Process(float in);
private:
	float freq_;
	float phase_;
	float phase_inc_;
	float sr_;
	float CalcPhaseInc(float f) {
		return (TWO_PI_F * f) * sr_;
	}
};

#endif
