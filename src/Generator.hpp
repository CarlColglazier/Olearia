#include <stdint.h>
#include <math.h>
#include "daisy.h"

class Oscillator {
public:
	Oscillator(float sr) {
		sr_ = sr;
		sr_recip_ = 1.0f / sr;
		freq_ = 440.0f;
		amp_ = 0.5f;
		phase_ = 0.0f;
	}
	~Oscillator() {}
	inline void SetFreq(float f) {
		freq_ = f;
		phase_inc_ = CalcPhaseInc(f);
	}
	inline void SetAmp(float a) {
		amp_ = a;
	}
	float Process();
	void PhaseAdd(float _phase) {
		phase_ += (_phase * float(M_TWOPI));
	}
private:
	float CalcPhaseInc(float f);
	float sr_, sr_recip_, freq_, amp_, phase_, phase_inc_;
};

// https://llllllll.co/t/digital-allpass-filters/27398
// https://ccrma.stanford.edu/~jos/pasp/Allpass_Filters.html
// https://ccrma.stanford.edu/~jos/pasp/One_Multiply_Scattering_Junctions.html
// https://github.com/madronalabs/madronalib/blob/master/source/DSP/MLDSPFilters.h (MIT)
class AllPassFilter {
public:
	float mCoeffs;
	//AllPassFilter(): mCoeffs(0.f){}
	AllPassFilter(float a) {
		//sr_ = sr;
		mCoeffs = coeffs(a);
	}
	~AllPassFilter() {}
	float coeffs(float d) {
		float xm1 = (d - 1.f);
		return -0.53f*xm1 + 0.24f*xm1*xm1;
	}
	float Process(float x) {
		float y = x1 + (x - y1)*mCoeffs;
		x1 = x;
		y1 = y;
		return y;
	}
private:
	float x1{0}, y1{0};
};

class OnePole {
public:
	float a0, b1, y1;
	OnePole(float omega) {
		y1 = 0;
		float x = expf(-omega*M_PI*2);
		a0 = 1.f - x;
		b1 = x;
	}
	float Process(float x) {
		y1 = a0 * x + b1 * y1;
		return y1;
	}
};



// A comb filter is a delay which continuously adds
// a past version of itself to a future version.
class CombFilter {
public:
	CombFilter(float sample_rate, float gain, float fund, daisy::RingBuffer<float, 48000> *rbp) {
		fund_ = fund;
		gain_ = gain;
		sr_ = sample_rate;
		rb = rbp;
		rb->Init();
		int delay = (int) (sr_ / fund_);
		for (int i = 0; i < delay; i++) {
			rb->Overwrite(0.0f);
		}
	}
	~CombFilter() {
		//delete &rb;
	}
	float Process(float x) {
		float out = rb->Read();
		rb->Write(x + gain_ * out);
		return out;
	}
private:
	daisy::RingBuffer<float, 48000> *rb;
	float fund_, gain_, sr_;
};
