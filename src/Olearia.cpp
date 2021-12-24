#include "daisy_patch.h"
#include "daisysp.h"
#include <math.h>
#include <string>

// local includes
#include "Generator.hpp"

using namespace daisy;
using namespace daisysp;

DaisyPatch patch;
float sample_rate;
int counter;
Parameter freq_in, shape_in, effect_in, wet_in;

// FX, generators, etc.
ReverbSc DSY_SDRAM_BSS reverb;
FmOscillator DSY_SDRAM_BSS fm;

void UpdateControls() {
	//patch.ProcessAllControls();
}

// AUDIO PROCESSOR
static void AudioThrough(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
	patch.ProcessAnalogControls();
	fm.SetFreq(freq_in.Process());
	fm.SetModIndex(shape_in.Process());
	for (size_t i = 0; i < size; i++) {
		out[0][i] = in[0][i]; // This is just a mult for now.
		out[1][i] = in[1][i] + fm.Process();
		reverb.Process(in[2][i], in[3][i], &out[2][i], &out[3][i]);
	}
}

void UpdateOled() {
	patch.ProcessDigitalControls();
	patch.display.Fill(false);
	// screensaver
	if (counter > 1000) {
		patch.display.Update();
		return;
	}
	patch.DisplayControls(false);
	//patch.display.DrawRect(50, 50, 85, 85, true, false);
	/*
	writeString(&patch, 50, 20, std::to_string(counter));
	writeString(&patch, 50, 30, std::to_string((int) shape_in.Value()));
	writeString(&patch, 50, 40, std::to_string((int) v_freq(freq_in.Value())));
	//writeString(&patch, 20, 50, std::to_string(patch.encoder.Pressed()));
	*/
	patch.display.Update();
}

int main(void) {
	patch.Init();
	//patch.seed.StartLog(false);

	// init controls
	freq_in.Init(patch.controls[patch.CTRL_1], 0.0f, 5.0f, Parameter::LINEAR);
	shape_in.Init(patch.controls[patch.CTRL_2], 0.0f, 5.0f, Parameter::LINEAR);
	effect_in.Init(patch.controls[patch.CTRL_2], 0.0f, 5.0f, Parameter::LINEAR);
	wet_in.Init(patch.controls[patch.CTRL_4], 0.0f, 100.0f, Parameter::LINEAR);

	patch.SetAudioBlockSize(48); // default
	// TODO: Things don't sound right with this sample rate for some reason.
	// See also: https://forum.electro-smith.com/t/revamped-audio-class-in-libdaisy/806
	//patch.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_96KHZ);

	sample_rate = patch.AudioSampleRate();
	counter = 0;
	// init voices
	fm.Init(sample_rate);
	reverb.Init(sample_rate);
	reverb.SetFeedback(0.85f);
    reverb.SetLpFreq(11000.0f);

	patch.StartAdc();
	patch.StartAudio(AudioThrough);
	
	// Warning: This needs to be called *after* StartAdc or the controls don't work...
	//patch.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_96KHZ);

	int last_update = patch.seed.system.GetNow();
	while(true) {
		//patch.DelayMs(1);
		if (patch.seed.system.GetNow() - last_update > 17) {
			UpdateOled();
			last_update = patch.seed.system.GetNow();
			counter++;
		}
	}
}
