#include "daisy_patch.h"
#include <string>
#include <cstring>
#include <math.h>
// local includes
#ifndef APPLET_
#define APPLET_
#include "Applet.hpp"
#endif

#define BUFF_SIZE 16
#define NUM_APPLETS 2

using namespace daisy;

static uint32_t DSY_QSPI_BSS buff[BUFF_SIZE];
//static uint32_t __attribute__((section(".dtcmram_bss"))) outbuff[BUFF_SIZE];
//static uint32_t  axi_outbuff[BUFF_SIZE];
uint32_t inbuff[BUFF_SIZE];


DaisyPatch patch;
int selected;
float sample_rate;
float controls[4];
int last_chan;

const int SCREEN_WIDTH = 128;

void writeString(int x, int y, std::string s) {
	patch.display.SetCursor(x, y);
	char* cstr = &s[0];
	patch.display.WriteString(cstr, Font_6x8, true);
}

enum App { VCO, VCA, NOISE, NUM_ITEMS };

class AppHost {
public:
	int position;
	App app;
	Applet *gen;
	AppHost() {
		position = 0;
		app = App::VCO;
		//Init();
	}

	void Init(float sample_rate) {
		switch (app) {
		case App::VCA:
			gen = new Amp(sample_rate);
			break;
		case App::VCO:
			gen = new Osc(sample_rate);
			break;
		case App::NOISE:
			gen = new Noise(sample_rate);
			break;
		default:
			gen = new Amp(sample_rate);
			break;
		}
	}

	void draw() {
		int draw_width = SCREEN_WIDTH / NUM_APPLETS;

		if (selected == position) {
			for (int x = position * draw_width; x < (position + 1) * draw_width; x++) {
				patch.display.DrawPixel(x, 30, true);
			}
		}
		const char *names[App::NUM_ITEMS] =
			{ "FM VCO", "VCA", "NOISE" };
		writeString(position * draw_width, 18, names[app]);
	}
};

AppHost *apphost[NUM_APPLETS];

/* persist settings */
void writeModes() {
	patch.seed.qspi_handle.mode = DSY_QSPI_MODE_INDIRECT_POLLING;
	dsy_qspi_init(&patch.seed.qspi_handle);
	for (int i = 0; i < NUM_APPLETS; i++) {
		//inbuff[i] = (uint32_t) applets[i].app;
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
	for (int i = 0; i < NUM_APPLETS; i++) {
		//applets[i].app = (App) buff[i];
	}
	dsy_qspi_deinit();
}

void UpdateControls() {
	// just pass along the values
	patch.UpdateAnalogControls();
	patch.DebounceControls();

	for (int i = 0; i < 4; i++) {
		controls[i] = patch.controls[i].Process();
	}

	if (patch.encoder.Pressed()) {
		// save?
		if (patch.encoder.TimeHeldMs() > 2000.0f && patch.encoder.TimeHeldMs() < 2010.0f) {
			//writeModes();
			// disabled for now
		}
		// encoder
		selected = (selected + patch.encoder.Increment()) % NUM_APPLETS;
		if (selected < 0) {
			selected = NUM_APPLETS + selected;
		}
	} else {
		int a = apphost[selected]->app + patch.encoder.Increment();
		if (a < 0) {
			a = App::NUM_ITEMS - 1;
		}
		if (a >= App::NUM_ITEMS) {
			a = 0;
		}
		// switch?
		if (apphost[selected]->app != (App) a) {
			delete apphost[selected]->gen;
			apphost[selected]->app = (App) a;
			apphost[selected]->Init(sample_rate);
		}
	}
}

// AUDIO PROCESSOR
static void AudioThrough(float **in, float **out, size_t size) {
	// audio stuff here
	for (int a = 0; a < NUM_APPLETS; a++) {
		apphost[a]->gen->Control(controls[2 * a], controls[2 * a + 1]);
		for (size_t i = 0; i < size; i++) {
			float *o;
			o = apphost[a]->gen->Process(in[2 * a][i], in[2 * a + 1][i]);
			out[2 * a][i] = o[0];
			out[2 * a + 1][i] = o[1];
			//out[2 * a + 1][i] = applets[a].gen->Process();
		}
	}
}

void UpdateOled() {
	patch.display.Fill(false);
	writeString(0, 0, "olearia");
	for (int i = 0; i < 128; i++) {
		patch.display.DrawPixel(i, 10, true);
	}
	for (int i = 0; i < NUM_APPLETS; i++) {
		apphost[i]->draw();
	}
	patch.display.Update();
}

void UpdateMidi() {
	patch.midi.Listen();
	if (patch.midi.HasEvents()) {
		MidiEvent me = patch.midi.PopEvent();
		if (me.type == ControlChange) {
			ControlChangeEvent cce = me.AsControlChange();
			last_chan = cce.value;
		}
	}
}

int main(void) {
	patch.Init();

	// set audio block size
	patch.SetAudioBlockSize(128);

	patch.StartAdc();
	patch.StartAudio(AudioThrough);
	patch.midi.StartReceive();
	last_chan = 0;

	sample_rate = patch.seed.AudioSampleRate();

	selected = 0;
	// init applets
	for (int i = 0; i < NUM_APPLETS; i++) {
		apphost[i] = new AppHost();
		apphost[i]->position = i;
		apphost[i]->Init(sample_rate);
	}

	// load settings
	//readModes();

	while(1) {
		UpdateControls();
		UpdateMidi();
		UpdateOled();
	}
}
