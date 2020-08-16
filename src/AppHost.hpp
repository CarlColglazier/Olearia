#include "daisy_patch.h"

#ifndef APPLET_
#define APPLET_
#include "Applet.hpp"
#endif

#define BUFF_SIZE 16
#define NUM_APPLETS 2

using namespace daisy;

enum App { VCO, VCA, NOISE, WAVESHAPER, NUM_ITEMS };
const int SCREEN_WIDTH = 100;//128;
const int S_WIDTH =  36; //(SCREEN_WIDTH / 2);

static uint32_t DSY_QSPI_BSS buff[BUFF_SIZE];
//static uint32_t __attribute__((section(".dtcmram_bss"))) outbuff[BUFF_SIZE];
//static uint32_t  axi_outbuff[BUFF_SIZE];
uint32_t inbuff[BUFF_SIZE];

void writeString(DaisyPatch patch, int x, int y, std::string s) {
	patch.display.SetCursor(x, y);
	char* cstr = &s[0];
	patch.display.WriteString(cstr, Font_6x8, true);
}

class AppHost {
public:
	int position;
	App app;
	Applet *gen;
	int dr[S_WIDTH * S_WIDTH];
	AppHost() {
		position = 0;
		app = App::VCO;
		for (int i = 0; i < S_WIDTH * S_WIDTH; i++) {
			dr[i] = 0;
		}
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
		case App::WAVESHAPER:
			gen = new Waveshaper(sample_rate);
			break;
		default:
			gen = new Amp(sample_rate);
			break;
		}
	}

	void draw(DaisyPatch patch, int selected) {
		int draw_width = SCREEN_WIDTH / NUM_APPLETS;
		int left_offset = position * draw_width;
		if (selected == position) {
			for (int x = left_offset; x < (position + 1) * draw_width; x++) {
				patch.display.DrawPixel(x, 12, true);
			}
		}
		const char *names[App::NUM_ITEMS] =
			{ "FM VCO", "VCA", "NOISE", "WVSHPR" };
		writeString(patch, position * draw_width, 2, names[app]);
		gen->Draw(dr, S_WIDTH, S_WIDTH);
		for (int x = 0; x < S_WIDTH; x++) {
			for (int y = 0; y < S_WIDTH; y++) {
				if (dr[S_WIDTH * y + x] == 1) {
					patch.display.DrawPixel(x + left_offset, y + 20, true);
				} else {
					patch.display.DrawPixel(x + left_offset, y + 20, false);
				}
			}
		}
		//delete dr;
	}
};

/* persist settings */
void writeModes(DaisyPatch patch, AppHost *apphost) {
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

void readModes(DaisyPatch patch) {
	patch.seed.qspi_handle.mode = DSY_QSPI_MODE_DSY_MEMORY_MAPPED;
	dsy_qspi_init(&patch.seed.qspi_handle);
	for (int i = 0; i < NUM_APPLETS; i++) {
		//applets[i].app = (App) buff[i];
	}
	dsy_qspi_deinit();
}
