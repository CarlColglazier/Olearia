#include "LPFilter.hpp"

void LPFilter::Init(float s_r) {
	sample_rate_ = s_r;
	cutoff = 440.0f;
	m_xnz1 = 0;
	m_xnz2 = 0;
	m_ynz1 = 0;
	m_ynz2 = 0;
	m_offset = 0;
	calc_cutoffs(cutoff);
}

void LPFilter::Control(float in) {
	cutoff = in * 20000.0f;
	calc_cutoffs(cutoff);
}

float LPFilter::Process(float in) {
	float xn = in;
	float yn = a0*xn + a1*m_xnz1 + a2*m_xnz2 - b1*m_ynz1 - b2*m_ynz2;
	m_xnz2 = m_xnz1;
	m_xnz1 = xn;
	m_ynz2 = m_ynz1;
	m_ynz1 = yn;
	return(yn + m_offset);
}

void LPFilter::calc_cutoffs(float cut) {
	float th = 2.0 * 3.141592 * cut / sample_rate_;
	float g = cosf(th) / (1.0 + sinf(th));
	a0 = (1.0 - g) / 2.0;
	a1 = (1.0 - g) / 2.0;
	a2 = 0.0;
	b1 = -g;
	b2 = 0.0;
}
