TARGET = olearia

CPP_SOURCES = src/Olearia.cpp src/Osc.cpp src/Noise.cpp src/LPFilter.cpp

LIBDAISY_DIR = ./libDaisy
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core

include $(SYSTEM_FILES_DIR)/Makefile
