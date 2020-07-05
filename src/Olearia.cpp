#include "daisy_patch.h"
#include <string>
#include <cstring>
#include <math.h>
// local includes
#ifndef GENERATOR_
#define GENERATOR_
#include "Generator.hpp"
#endif

// import generators
#include "Amp.hpp"
#include "Osc.hpp"
#include "Noise.hpp"
#include "LPFilter.hpp"

#define BUFF_SIZE 16

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

enum App {
					VCA,
					VCO,
					NOISE,
					VCF,
					NUM_ITEMS
};

class Applet {
public:
	int position;
	App app;
	Generator *gen;
	Applet() {
		position = 0;
		app = App::VCA;
		Init();
	}

	void Init() {
		switch (app) {
		case App::VCA:
			gen = new Amp();
			break;
		case App::VCO:
			gen = new Osc();
			break;
		case App::NOISE:
			gen = new Noise();
			break;
		case App::VCF:
			gen = new LPFilter();
			break;
		default:
			gen = new Amp();
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
		const char *names[App::NUM_ITEMS] =
			{ "VCA", "VCO", "NOISE", "VCF" };
		writeString(position * draw_width, 20, names[app]);
	}
};


Applet applets[4];

/* persist settings */
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
		applets[a].gen->Control(controls[a]);
		for (size_t i = 0; i < size; i++) {
			out[a][i] = applets[a].gen->Process(in[a][i]);
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
			delete applets[selected].gen;
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
