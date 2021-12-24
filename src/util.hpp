#pragma once

#ifndef UTIL_H_
#define UTIL_H_

#include "daisy_patch.h"

using namespace daisy;

#include <string>

#define M_PI       3.14159265358979323846

void writeString(DaisyPatch* patch, int x, int y, std::string s);

float v_freq(float v);

#endif