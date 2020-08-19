#include "daisy_patch.h"

#include <random>
#include <math.h>
#include "Generator.hpp"

using namespace daisy;

#ifndef DELAYS_
#define DELAYS_
static daisy::RingBuffer<float, 100000> DSY_SDRAM_BSS delay1;
static daisy::RingBuffer<float, 100000> DSY_SDRAM_BSS delay2;
static daisy::RingBuffer<float, 100000> DSY_SDRAM_BSS delay3;
static daisy::RingBuffer<float, 100000> DSY_SDRAM_BSS delay4;
static daisy::RingBuffer<float, 100000> DSY_SDRAM_BSS delay5;
static daisy::RingBuffer<float, 100000> DSY_SDRAM_BSS delay6;
static daisy::RingBuffer<float, 100000> DSY_SDRAM_BSS delay7;
#endif

class Applet {
public:
	virtual ~Applet() {};
	virtual void Control(float f_l, float f_r) = 0;
	virtual void Process(float *in, float *out, size_t s) = 0;
	virtual void Draw(int *d, int width, int height) = 0;
};

class Amp: public Applet {
public:
	Amp(float sample_rate);
	void Control(float in_l, float in_r);
	void Process(float *in, float *out, size_t s);
	void Draw(int *d, int width, int height);
private:
	float amp_l, amp_r;
};

class Noise: public Applet {
public:
	Noise(float sample_rate);
	void Control(float in_l, float in_r);
	void Process(float *in, float *out, size_t s);
	void Draw(int *d, int width, int height);
private:
	float amp_;
};

constexpr float TWO_PI_F = (float)M_TWOPI;

// just a sine wave for now.
// a lot of the design here is taken from DaisySP
class Osc: public Applet {
public:
	Osc(float sample_rate);
	~Osc();
	void Control(float in_l, float in_r);
	void Process(float *in, float *out, size_t s);
	void Draw(int *d, int width, int height);
private:
	float car_freq_, mod_freq_, mod_index_;
	Oscillator *osc_one;
	Oscillator *osc_two;
};

class Waveshaper: public Applet {
public:
	Waveshaper(float sample_rate);
	void Control(float in_l, float in_r);
	void Process(float *in, float *out, size_t s);
	void Draw(int *d, int width, int height);
private:
	float in_l, in_r;
};

class JCReverb: public Applet {
public:
	JCReverb(float sample_rate) {
		sr_ = sample_rate;
		dl1 = new DelayLine(0.3f, 1051);
		dl2 = new DelayLine(0.3f, 337);
		ap1 = new AllPassFilter(sample_rate, 0.7f, 1051, &delay1);
		ap2 = new AllPassFilter(sample_rate, 0.7f, 337, &delay2);
		ap3 = new AllPassFilter(sample_rate, 0.7f, 113, &delay3);
		comb1 = new CombFilter(sample_rate, 0.742, 901, &delay4);
		comb2 = new CombFilter(sample_rate, 0.733, 778, &delay5);
		comb3 = new CombFilter(sample_rate, 0.715, 1011, &delay6);
		comb4 = new CombFilter(sample_rate, 0.697, 1123, &delay7);
		wet_ = 0.5f;
		//comb3 = new CombFilter(sample_rate, 0.715, 13.0f);
	}
	~JCReverb() {
		delete dl1;
		delete dl2;
		delete ap1;
		delete ap2;
		delete ap3;
		delete comb1;
		delete comb2;
		delete comb3;
		delete comb4;
	}
	void Control(float in_l, float in_r) {
		wet_ = in_l;
	}
	void Process(float *in, float *out, size_t s) {
		float ii = in[0];
		/*
		ii = ap1->Process(ii);
		ii = ap2->Process(ii);
		ii = ap3->Process(ii);
		float o1 = comb1->Process(ii);
		float o2 = comb2->Process(ii);
		float o3 = comb3->Process(ii);
		float o4 = comb4->Process(ii);
		//ii = (o1 + o2 + o3 + o4) / 4.0f;
		//
		*/
		ii = dl1->Process(ii);
		ii = dl2->Process(ii);
		float w = ii;//(o1 + o2 + o3 + o4) / 4.0f;
		float o = w * wet_ + (1 - wet_) * in[0];
		/*
		if (o > 1.0f) {
			o = 1.0f;
		}
		if (o < -1.0f) {
			o = -1.0f;
			}*/
		out[0] = o;
		out[1] = 0.0f;
	}
	void Draw(int *d, int width, int height) {
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				d[y*width+x] = 0;
			}
		}
	}
private:
	DelayLine *dl1;
	DelayLine *dl2;
	AllPassFilter *ap1;
	AllPassFilter *ap2;
	AllPassFilter *ap3;
	CombFilter *comb1;
	CombFilter *comb2;
	CombFilter *comb3;
	CombFilter *comb4;
	float sr_, wet_;
	//CombFilter *comb3;
	//AllPassFilter* aps[3];
	//OnePole op[4];
};

