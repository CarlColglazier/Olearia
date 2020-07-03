#include "daisy_patch.h"
#include <string>
#include <random>
#include <cstring>
#include <math.h>

#define BUFF_SIZE 16

constexpr float TWO_PI_F     = (float)M_TWOPI;
constexpr float TWO_PI_RECIP = 1.0f / TWO_PI_F;

using namespace daisy;

static uint32_t DSY_QSPI_BSS buff[BUFF_SIZE];
//static uint32_t __attribute__((section(".dtcmram_bss"))) outbuff[BUFF_SIZE];
//static uint32_t  axi_outbuff[BUFF_SIZE];
uint32_t inbuff[BUFF_SIZE];


DaisyPatch patch;
int selected;
float sample_rate;

const int SCREEN_WIDTH = 128;

void writeString(int x, int y, std::string s) {
	patch.display.SetCursor(x, y);
	char* cstr = &s[0];
	patch.display.WriteString(cstr, Font_7x10, true);
}

class Generator {
public:
	virtual void Init(float sample_rate) = 0;
	virtual float Process() = 0;
};

class Noise: public Generator {
public:
	void Init(float sample_rate) {
	}
	float Process() {
		return rand() / (RAND_MAX + 1.);
	}
};

// just a sine wave for now.
// a lot of the design here is taken from DaisySP
class Osc: public Generator {
public:
	void Init(float sample_rate) {
		freq_ = 220.0f;
		phase_ = 0.0f;
		phase_inc_ = CalcPhaseInc(freq_);
	};
	float Process() {
		float out;
		out = sinf(phase_);
		phase_ += phase_inc_;
		if (phase_ > TWO_PI_F) {
			phase_ -= TWO_PI_F;
		}
		return out;
	};
private:
	float freq_;
	float phase_;
	float phase_inc_;
	float CalcPhaseInc(float f) {
		return (TWO_PI_F * f) * (1.0f / sample_rate);
	}
};

enum App {
					VCA,
					VCO,
					NOISE,
					NUM_ITEMS
};

class Applet {
public:
	int position;
	App app;
	Generator *gen;
	//Applet(int position);
	Applet() {
		position = 0;
		app = App::VCA;
		Init();
	}

	void Init() {
		switch (app) {
		case App::VCO:
			gen = new Osc();
			break;
		case App::NOISE:
			gen = new Noise();
			break;
		default:
			return;
			//gen = new Osc();
			break;
		}
		gen->Init(sample_rate);
	}
	
	void draw() {
		int draw_width = SCREEN_WIDTH / 4;

		if (selected == position) {
			for (int x = position * draw_width; x < (position + 1) * draw_width; x++) {
				patch.display.DrawPixel(x, 35, true);
			}
		}
		switch(app) {
		case VCA:
			writeString(position * draw_width, 20, "VCA");
			break;
		case VCO:
			writeString(position * draw_width, 20, "VCO");
			break;
		case NOISE:
			writeString(position * draw_width, 20, "NS");
			break;
		default:
			break;
		}
	}
};



Applet applets[4];

void writeModes() {
	patch.seed.qspi_handle.mode = DSY_QSPI_MODE_INDIRECT_POLLING;
	dsy_qspi_init(&patch.seed.qspi_handle);
	for (int i = 0; i < 4; i++) {
		inbuff[i] = (uint32_t) applets[i].app;
	}
	uint32_t base = 0x90000000;
	uint32_t writesize = BUFF_SIZE * sizeof(buff[0]);
	dsy_qspi_erase(base, base + writesize);
	dsy_qspi_write(base, writesize, (uint8_t*)inbuff);
	dsy_qspi_deinit();
}

void readModes() {
	patch.seed.qspi_handle.mode = DSY_QSPI_MODE_DSY_MEMORY_MAPPED;
	dsy_qspi_init(&patch.seed.qspi_handle);
	for (int i = 0; i < 4; i++) {
		applets[i].app = (App) buff[i];
	}
	dsy_qspi_deinit();
}

// AUDIO PROCESSOR
static void AudioThrough(float **in, float **out, size_t size) {
	// just pass along the values
	patch.UpdateAnalogControls();
	patch.DebounceControls();

	float controls[4];
	for (int i = 0; i < 4; i++) {
		controls[i] = patch.controls[i].Process();
	}

	// audio stuff here
	for (int a = 0; a < 4; a++) {
		for (size_t i = 0; i < size; i++) {
			if (applets[a].app == App::VCA) {
				out[a][i] = in[a][i] * controls[a];
			} else {
				out[a][i] = applets[a].gen->Process();
			}
		}
	}
	
	if (patch.encoder.Pressed()) {
		// save?
		if (patch.encoder.TimeHeldMs() > 2000.0f && patch.encoder.TimeHeldMs() < 2010.0f) {
			writeModes();
		}
		// encoder
		selected = (selected + patch.encoder.Increment()) % 4;
		if (selected < 0) {
			selected = 4 + selected;
		}
	} else {
		int a = applets[selected].app + patch.encoder.Increment();
		if (a < 0) {
				a = App::NUM_ITEMS - 1;
		}
		if (a >= App::NUM_ITEMS) {
			a = 0;
		}
		// switch?
		if (applets[selected].app != (App) a) {
			applets[selected].app = (App) a;
			applets[selected].Init();
		}
	}
}

void UpdateOled() {
	patch.display.Fill(false);

	writeString(0, 0, "olearia");

	for (int i = 0; i < 128; i++) {
		patch.display.DrawPixel(i, 15, true);
	}

	for (int i = 0; i < 4; i++) {
		applets[i].draw();
	}
	
	patch.display.Update();
}

int main(void) {
	patch.Init();
	
	patch.StartAdc();
	patch.StartAudio(AudioThrough);

	sample_rate = patch.seed.AudioSampleRate();

	selected = 0;
	// init applets
	for (int i = 0; i < 4; i++) {
		applets[i].position = i;
		//applets[i].Init();
	}

	// load settings
	//readModes();
	
	while(1) {
		UpdateOled();
	}
}
