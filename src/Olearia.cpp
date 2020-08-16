#include "daisy_patch.h"
#include <string>
#include <cstring>
#include <math.h>

// local includes
#ifndef APPLET_
#define APPLET_
#include "Applet.hpp"
#endif

#ifndef APPHOST_
#define APPHOST_
#include "AppHost.hpp"
#endif

using namespace daisy;

DaisyPatch patch;
int selected;
float sample_rate;
float controls[4];
int last_chan;

AppHost *apphost[NUM_APPLETS];

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
	//writeString(patch, 0, 0, "olearia");
	for (int i = 0; i < 128; i++) {
		patch.display.DrawPixel(i, 10, true);
	}
	for (int i = 0; i < NUM_APPLETS; i++) {
		apphost[i]->draw(patch, selected);
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
