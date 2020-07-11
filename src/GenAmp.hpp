#ifndef GENERATOR_
#define GENERATOR_
#include "Generator.hpp"
#endif

#ifndef VCA_H
#define VCA_H

class Amp: public Generator {
public:
	void Init(float sample_rate);
	void Control(float in);
	float Process(float in);
private:
	float amp_;
};


#endif
