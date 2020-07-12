TARGET = olearia

CPP_SOURCES = src/Olearia.cpp src/GenAmp.cpp src/GenOsc.cpp src/GenNoise.cpp src/GenLPFilter.cpp

LIBDAISY_DIR = ./libDaisy
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core

include $(SYSTEM_FILES_DIR)/Makefile

.ONESHELL:
daisy:
	cd $(LIBDAISY_DIR)
	make
