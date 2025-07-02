TARGET = olearia
SRC_DIR=src
USE_DAISYSP_LGPL = 1
CPP_SOURCES = $(wildcard $(SRC_DIR)/*cpp)

LIBDAISY_DIR = ./lib/libDaisy
DAISYSP_DIR = ./lib/DaisySP
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
