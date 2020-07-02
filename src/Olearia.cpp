#include "daisy_patch.h"
#include <string>
#include <random>

using namespace daisy;

DaisyPatch patch;
int selected;

const int SCREEN_WIDTH = 128;

void writeString(int x, int y, std::string s) {
	patch.display.SetCursor(x, y);
	char* cstr = &s[0];
	patch.display.WriteString(cstr, Font_7x10, true);
}

enum App {
					VCA,
					NOISE,
					NUM_ITEMS
};

class Applet {
public:
	int position;
	App app;
	//Applet(int position);
	Applet() {
		position = 0;
		app = App::VCA;
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
		case NOISE:
			writeString(position * draw_width, 20, "NOISE");
			break;
		default:
			break;
		}
	}
};

Applet applets[4];

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
			switch(applets[a].app) {
			case App::VCA:
				out[a][i] = in[a][i] * controls[a];
				break;
			default:
				out[a][i] = rand() / (RAND_MAX + 1.);
				break;
			}
		}
	}
	
	if (patch.encoder.Pressed()) {
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
		applets[selected].app = (App) a;
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

	selected = 0;
	// init applets
	for (int i = 0; i < 4; i++) {
		applets[i].position = i;
	}

	//patch.ChangeAudioCallback(AudioThrough);
	
	while(1) {
		UpdateOled();
	}
}
