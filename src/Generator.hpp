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


// A comb filter is a delay which continuously adds
// a past version of itself to a future version.
class CombFilter {
public:
	CombFilter(float gain, int samples, float *buff_start) {
		gain_ = gain;
		samples_ = samples;
		buff = buff_start;
		/*
		for (int i = 0; i < 6000; i++) {
			buff[i] = 0.0f;
		}
		*/
		read_pos = 0;
		write_pos = samples;
	}
	~CombFilter() {}
	float Process(float x) {
		float out = buff[read_pos];
		//float out = rb->Read();
		float y = x + gain_ * out;
		write_pos = (read_pos + samples_) % 6000;
		buff[write_pos] = y;
		read_pos = (read_pos + 1) % 6000;
		return out;
	}
private:
	float gain_;
	float *buff;
	int read_pos, write_pos, samples_;
};

class DelayLine {
public:
	DelayLine(int samples, float *buff_start) {
		samples_ = samples;
		buff = buff_start;
		/*
		for (int i = 0; i < 3000; i++) {
			buff[i] = 0.0f;
		}
		*/
		read_pos = 0;
		write_pos = samples;
	}
	~DelayLine() {}
	float Process(float x) {
		float read = buff[read_pos];
		write_pos = (read_pos + samples_) % 3000;
		buff[write_pos] = x;
		// update pointers
		read_pos = (read_pos + 1) % 3000;
		return read;
	}
private:
	float *buff;
	int read_pos, write_pos, samples_;
};

class AllPassFilter {
public:
	AllPassFilter(float fb, int samples, float *buff_start) {
		samples_ = samples;
		fb_ = fb;
		buff = buff_start;
		for (int i = 0; i < 2500; i++) {
			buff[i] = 0.0f;
		}
		read_pos = 0;
		write_pos = samples;
	}
	~AllPassFilter() {}
	// https://christianfloisand.wordpress.com/tag/all-pass-filter/
	// https://dsp.stackexchange.com/questions/19998/allpass-filter-feedforward-feedback-design-and-code
	float Process(float x) {
		//float g = 0.7f;
		float feedback = fb_;
		// https://github.com/djzielin/bucket-drums/blob/8a85cf389462aa78b477fb0a40fea599210b5e11/allpass_filter.cpp
		float read = buff[read_pos];
		float delay_input = x - feedback * read;
		write_pos = (read_pos + samples_) % 2500;
		buff[write_pos] = delay_input;
		return delay_input * feedback + read;
	}
private:
	int read_pos, write_pos, samples_;
	float fb_;
	float *buff;
};
