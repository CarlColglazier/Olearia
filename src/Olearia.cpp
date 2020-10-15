#include "daisy_patch.h"
#include <math.h>
#include <string>

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
int wait;

AppHost *apphost[NUM_APPLETS];
int counter;

void UpdateControls() {
	// just pass along the values
	patch.UpdateAnalogControls();
	patch.DebounceControls();

	for (int i = 0; i < 4; i++) {
		controls[i] = patch.controls[i].Process();
	}

	if (patch.encoder.Pressed()) {
		// wake up
		counter = 0;
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
			wait = 10;
			apphost[selected]->Init((App) a, sample_rate);
		}
	}
}

// AUDIO PROCESSOR
static void AudioThrough(float **in, float **out, size_t size) {
	// Some things take a little bit of time to load.
	// This avoids a runtime error, which is a huge headache!
	if (wait > 0) {
		wait--;
		return;
	}
	// audio stuff here
	for (int a = 0; a < NUM_APPLETS; a++) {
		apphost[a]->gen->Control(controls[2 * a], controls[2 * a + 1]);
		for (size_t i = 0; i < size; i++) {
			float ii[2];
			float o[2];
			ii[0] = in[2 * a][i];
			ii[1] = in[2 * a + 1][i];
			apphost[a]->gen->Process(ii, o, 2);
			out[2 * a][i] = o[0];
			out[2 * a + 1][i] = o[1];
		}
	}
}

void UpdateOled() {
	patch.display.Fill(false);
	// screensaver
	if (counter > 10000) {
		patch.display.Update();
		return;
	}
	for (int i = 0; i < NUM_APPLETS; i++) {
		apphost[i]->draw(patch, selected);
	}
	writeString(patch, 100, 20, std::to_string(counter));

	patch.display.Update();
	counter++;
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
	patch.SetAudioBlockSize(256);

	patch.StartAdc();
	patch.StartAudio(AudioThrough);
	patch.midi.StartReceive();
	last_chan = 0;

	sample_rate = patch.seed.AudioSampleRate();

	selected = 0;
	wait = 0;
	counter = 0;
	// init applets
	for (int i = 0; i < NUM_APPLETS; i++) {
		apphost[i] = new AppHost();
		apphost[i]->position = i;
		apphost[i]->Init(App::VCO, sample_rate);
	}

	// load settings
	//readModes();

	while(1) {
		UpdateControls();
		UpdateMidi();
		UpdateOled();
	}
}
