#include "util.hpp"

/**
 * @brief Shortcut function to write strings and handle position.
 * 
 * @param patch Hardware
 * @param x position
 * @param y position
 * @param s string
 */
void writeString(DaisyPatch* patch, int x, int y, std::string s) {
	patch->display.SetCursor(x, y);
	char* sp = &s[0];
	patch->display.WriteString(sp, Font_6x8, true);
}

float v_freq(float v) {
	// C3 is the base note.
	float volts = v; // 5.0f * v;
	float midi_note = volts * 12.0f + 48.0f;
	return (440.0f) * powf(2.0, (midi_note - 69.0f) / 12.0f);
}