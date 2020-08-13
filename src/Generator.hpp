#include <stdint.h>
#include <math.h>

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
