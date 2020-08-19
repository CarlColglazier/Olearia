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
/*
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
*/

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
	CombFilter(float sample_rate, float gain, int samples, daisy::RingBuffer<float, 100000> *rbp) {
		//fund_ = fund;
		samples_ = samples;
		gain_ = gain;
		sr_ = sample_rate;
		rb = rbp;
		rb->Init();
		int delay = samples_;//(int) (sr_ / fund_);
		if (delay > rb->writable()) {
			delay = rb->writable();
		}
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
	daisy::RingBuffer<float, 100000> *rb;
	float gain_, sr_;
	int samples_;
};

class DelayLine {
public:
	DelayLine(float fb, int samples) {
		fb_ = fb;
		samples_ = samples;
		for (int i = 0; i < samples; i++) {
			buff[i] = 0.0f;
		}
		read_pos = 0;
		write_pos = samples;
	}
	~DelayLine() {}
	float Process(float x) {
		float feedback = fb_;
		float read = buff[read_pos];
		float delay_input = x + feedback * read;
		write_pos = (read_pos + samples_) % 10000;
		buff[write_pos] = delay_input;
		// update pointers
		read_pos = (read_pos + 1) % 10000;
		return delay_input * feedback + read;
	}
private:
	float fb_;
	float buff[10000];
	int read_pos, write_pos, samples_;
};

class AllPassFilter {
public:
	AllPassFilter(float sample_rate, float fb, int samples, daisy::RingBuffer<float, 100000> *rbp) {
		d_ = samples;
		sr_ = sample_rate;
		rb = rbp;
		fb_ = fb;
		rb->Init();
		int delay = d_;
		if (delay > rb->writable()) {
			delay = rb->writable();
		}
		for (int i = 0; i < delay; i++) {
			rb->Overwrite(0.0f);
		}
	}
	~AllPassFilter() {}
	// https://christianfloisand.wordpress.com/tag/all-pass-filter/
	// https://dsp.stackexchange.com/questions/19998/allpass-filter-feedforward-feedback-design-and-code
	float Process(float x) {
		//float g = 0.7f;
		float feedback = fb_;
		// https://github.com/djzielin/bucket-drums/blob/8a85cf389462aa78b477fb0a40fea599210b5e11/allpass_filter.cpp
		float read = rb->ImmediateRead();
		float delay_input = x - feedback * read;
		rb->Write(delay_input);
		return delay_input * feedback + read;
	}
private:
	int d_, sr_;
	float fb_;
	daisy::RingBuffer<float, 100000> *rb;
};
