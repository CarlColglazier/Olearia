#include "Applet.hpp"

// AMP
// ----------------------
Amp::Amp(float sample_rate) {
	amp_l = 0.0f;
	amp_r = 0.0f;
}

void Amp::Control(float in_l, float in_r) {
	amp_l = in_l;
	amp_r = in_r;
}

void Amp::Process(float *in, float *out, size_t s) {
	out[0] = in[0] * amp_l;
	out[1] = in[1] * amp_r;
}

void Amp::Draw(int *out, int width, int height) {
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			out [y * width + x] = 0;
			if (x > 0 && x < width / 2 && y > (height - amp_l * height)) {
				out[y * width + x] = 1;
			}
			if (x > width / 2 && y > (height - amp_r * height)) {
				out[y * width + x] = 1;
			}
		}
	}
}

// NOISE
// ----------------------
Noise::Noise(float sample_rate) {
	amp_ = 1.0;
}

void Noise::Control(float in_l, float in_r) {
	amp_ = in_l;
}

void Noise::Process(float *in, float *out, size_t s) {
	float v = (rand() / (RAND_MAX + 1.)) * amp_;
	out[0] = v;
	out[1] = v;
}

void Noise::Draw(int *out, int width, int height) {
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			if (rand() % 2 == 0) {
				out [y * width + x] = 0;
			} else {
				out [y * width + x] = 1;
			}
		}
	}
}

// OSC
// ----------------------
Osc::Osc(float sample_rate) {
	car_freq_ = 440.0f;
	mod_freq_ = 110.0f;
	mod_index_ = 100.0;
	osc_one = new Oscillator(sample_rate);
	osc_two = new Oscillator(sample_rate);
}

Osc::~Osc() {
	delete osc_one;
	delete osc_two;
}

float v_to_freq(float v) {
	// C3 is the base note.
	float volts = 5.0f * v;
	float midi_note = volts * 12.0f + 48.0f;
	return (440.0f) * powf(2.0, (midi_note - 69.0f) / 12.0f);
}

void Osc::Control(float in_l, float in_r) {
	// quantize.
	int il = (int) (in_l * 60.0f);
	float il_f = ((float) il) / 60.0f;
	car_freq_ = v_to_freq(il_f);
	//mod_freq_ = v_to_freq(in_r);
	mod_index_ = in_r * 5000.0f;
}

void Osc::Process(float *in, float *out, size_t s) {
	// FM
	osc_one->SetFreq(car_freq_);
	float mod_f = osc_one->Process();
	osc_two->SetFreq(mod_f * mod_index_ + car_freq_);
	out[0] = mod_f;
	out[1] = osc_two->Process() * 0.5f; // amp at the end.
}

void Osc::Draw(int *out, int width, int height) {
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			out [y * width + x] = 0;
		}
	}
}

// Waveshaper
// ----------------------
Waveshaper::Waveshaper(float sample_rate) {
	in_l = 0.0f;
	in_r = 0.0f;
	sr_ = sample_rate;
}

void Waveshaper::Control(float l, float r) {
	in_l = l;
	in_r = r;
}

void Waveshaper::Process(float *in, float *out, size_t s) {
	float l = in[0] * (in_l + 1.0) * 10;
	float r = in[1] * (in_r + 1.0) * 10;
	float p = (in_r * 0.5) + 0.25;
	// from https://www.desmos.com/calculator/ge2wvg2wgj
	// via https://www.kvraudio.com/forum/viewtopic.php?t=501471
	// This is pretty basic and I will probably change it in the
	// future to be more flexible.
	out[0] = 4*(abs(p*l + p - round(p * l + p)) - p);
	out[1] = 4*(abs(0.25*r + 0.25 - round(0.25 * r + 0.25)) - 0.25);
}

void Waveshaper::Draw(int *out, int width, int height) {
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			out [y * width + x] = 0;
			if (x > 0 && x < width / 2 && y > (height - in_l * height)) {
				out[y * width + x] = 1;
			}
			if (x > width / 2 && y > (height - in_r * height)) {
				out[y * width + x] = 1;
			}
		}
	}
}
