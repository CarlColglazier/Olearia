#pragma once

#ifndef OLERIA_GENERATOR_H
#define OLERIA_GENERATOR_H

#include <stdint.h>
#include <math.h>
#include "daisy.h"
#include "util.hpp"

#ifndef M_TWOPI
#define M_TWOPI 6.2831853071795862319959
#endif

class SinOscillator {
public:
	SinOscillator() {
		freq_ = 440.0f;
		amp_ = 0.5f;
		phase_ = 0.0f;
	}
	~SinOscillator() {}
    inline void Init(float sr) {
        sr_ = sr;
        sr_recip_ = 1.0f / sr;
    }
	inline void SetFreq(float f) {
		freq_ = f;
		phase_inc_ = CalcPhaseInc(f);
	}
	inline void SetAmp(float a) {
		amp_ = a;
	}
	float Process();
	void PhaseAdd(float _phase) {
		phase_ += (_phase * M_TWOPI);
	}
private:
	float CalcPhaseInc(float f);
	float sr_, freq_, amp_, phase_, phase_inc_, sr_recip_;
};

class FmOscillator {
    public:
    FmOscillator() {
        car_freq_ = 440.0f;
    	mod_freq_ = 110.0f;
    	mod_index_ = 100.0;
    }
    ~FmOscillator() {}
    inline void Init(float sr) {
        sr_ = sr;
        osc_one.Init(sr);
        osc_two.Init(sr);
    }
    inline void SetFreq(float f) {
        // quantize
        int il = (int) (f * 60.0f);
        float il_f = ((float) il) / 60.0f;
    	car_freq_ = v_freq(il_f);
    }
    inline void SetModIndex(float f) {
        mod_index_ = f * 1000.0f;
    }
    inline void SetAmp(float a) {}
    float Process();
    void PhaseAdd(float _phase) {}
    private:
    float sr_, car_freq_, mod_freq_, mod_index_;
    SinOscillator osc_one, osc_two;
};

#endif