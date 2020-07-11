#ifndef GENERATOR_
#define GENERATOR_
#include "Generator.hpp"
#endif

#include <math.h>

class LPFilter: public Generator {
public:
	void Init(float s_r);
	void Control(float in);
	float Process(float in);
private:
	void calc_cutoffs(float cut);
	float cutoff;
	float a0, a1, a2, b1, b2, c0, d0;
	float m_xnz1, m_xnz2, m_ynz1, m_ynz2, m_offset;
	float sample_rate_;
};
