#include "daisy_patch.h"

#include <random>
#include <math.h>
#include "Generator.hpp"

using namespace daisy;

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
	JCReverb(float sample_rate, int position) {
		sr_ = sample_rate;
		dl1 = new DelayLine(2208, &sdbuff[0 + 500000*position]);
		dl2 = new DelayLine(2736, &sdbuff[10000 + 500000*position]);
		dl3 = new DelayLine(1968, &sdbuff[20000 + 500000*position]);
		dl4 = new DelayLine(2592, &sdbuff[30000 + 500000*position]);
		ap1 = new AllPassFilter(0.7f, 1051, &sdbuff[40000 + 500000*position]);
		ap2 = new AllPassFilter(0.7f, 337, &sdbuff[50000 + 500000*position]);
		ap3 = new AllPassFilter(0.7f, 113, &sdbuff[60000 + 500000*position]);
		comb1 = new CombFilter(0.742, 4799, &sdbuff[70000 + 500000*position]);
		comb2 = new CombFilter(0.733, 4999, &sdbuff[80000 + 500000*position]);
		comb3 = new CombFilter(0.715, 5399, &sdbuff[90000 + 500000*position]);
		comb4 = new CombFilter(0.697, 5801, &sdbuff[100000 + 500000*position]);
		wet_ = 0.5f;
	}
	~JCReverb() {
		delete dl1;
		delete dl2;
		delete dl3;
		delete dl4;
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

		ii = ap1->Process(ii);
		ii = ap2->Process(ii);
		ii = ap3->Process(ii);
		float o1 = dl1->Process(comb1->Process(ii));
		float o2 = dl2->Process(comb2->Process(ii));
		float o3 = dl3->Process(comb3->Process(ii));
		float o4 = dl4->Process(comb4->Process(ii));
		float w = (o1 + o2 + o3 + o4) / 4.0f;
		float o = w * wet_ + (1 - wet_) * in[0];
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
	DelayLine *dl3;
	DelayLine *dl4;
	AllPassFilter *ap1;
	AllPassFilter *ap2;
	AllPassFilter *ap3;
	CombFilter *comb1;
	CombFilter *comb2;
	CombFilter *comb3;
	CombFilter *comb4;
	float sr_, wet_;
};

