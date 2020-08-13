TARGET = olearia

SRC_DIR=src

CPP_SOURCES = $(wildcard $(SRC_DIR)/*cpp)

LIBDAISY_DIR = ./lib/libDaisy
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core

include $(SYSTEM_FILES_DIR)/Makefile

.ONESHELL:
daisy:
	cd $(LIBDAISY_DIR)
	make

deploy: daisy build program
