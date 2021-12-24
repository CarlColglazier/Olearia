TARGET = olearia

SRC_DIR=src

CPP_SOURCES = $(wildcard $(SRC_DIR)/*cpp)

LIBDAISY_DIR = ./lib/libDaisy
DAISYSP_DIR = ./lib/DaisySP
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core

include $(SYSTEM_FILES_DIR)/Makefile

libdaisy:
	cd $(LIBDAISY_DIR) && make
daisysp:
	cd $(DAISYSP_DIR) && make

lib: libdaisy daisysp
deploy: lib build program
