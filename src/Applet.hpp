#include "daisy_patch.h"

#include <random>
#include <math.h>
#include "Generator.hpp"

using namespace daisy;

class Applet {
public:
	virtual ~Applet() {};
	virtual void Control(float f_l, float f_r) = 0;
	virtual float* Process(float f_l, float f_r) = 0;
	virtual void Draw(int *d, int width, int height) = 0;
};

class Amp: public Applet {
public:
	Amp(float sample_rate);
	void Control(float in_l, float in_r);
	float* Process(float in, float in_r);
	void Draw(int *d, int width, int height);
private:
	float amp_l, amp_r;
};

class Noise: public Applet {
public:
	Noise(float sample_rate);
	void Control(float in_l, float in_r);
	float* Process(float in_l, float in_r);
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
	float* Process(float in_l, float in_r);
	void Draw(int *d, int width, int height);
private:
	float car_freq_, mod_freq_, mod_index_;
	Oscillator *osc_one;
	Oscillator *osc_two;
};

