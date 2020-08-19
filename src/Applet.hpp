#include "daisy_patch.h"

#include <random>
#include <math.h>
#include "Generator.hpp"

using namespace daisy;

#ifndef DELAYS_
#define DELAYS_
static daisy::RingBuffer<float, 48000> DSY_SDRAM_BSS delay1;
static daisy::RingBuffer<float, 48000> DSY_SDRAM_BSS delay2;
static daisy::RingBuffer<float, 48000> DSY_SDRAM_BSS delay3;
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
		comb1 = new CombFilter(sample_rate, 0.742, 13.0f, &delay1);
		comb2 = new CombFilter(sample_rate, 0.733, 15.0f, &delay2);
		wet_ = 0.5f;
		//comb3 = new CombFilter(sample_rate, 0.715, 13.0f);
	}
	~JCReverb() {
		delete comb1;
		delete comb2;
		//delete comb3;
	}
	void Control(float in_l, float in_r) {
		wet_ = in_l;
	}
	void Process(float *in, float *out, size_t s) {
		float o1 = comb1->Process(in[0]);
		float o2 = comb2->Process(in[0]);
		float w = (o1 + o2) / 2.0f;
		out[0] = w * wet_ + (1 - w) * in[0];
	}
	void Draw(int *d, int width, int height) {
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				d[y*width+x] = 0;
			}
		}
	}
private:
	CombFilter *comb1;
	CombFilter *comb2;
	int w;
	float sr_, wet_;
	//CombFilter *comb3;
	//AllPassFilter* aps[3];
	//OnePole op[4];
};

