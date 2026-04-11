# Noise Plethora Firmware — Build & Flash
# Requires: arduino-cli, teensy:avr core, patched AudioStream files
#
# Usage:
#   make          — compile firmware
#   make upload   — compile + flash to connected Teensy
#   make clean    — remove build artifacts
#   make port     — show available serial ports

FQBN       := teensy:avr:teensy40
SKETCH_DIR := /tmp/Noise_Plethora
SRC_DIR    := $(shell pwd)
PORT       ?= $(shell arduino-cli board list 2>/dev/null | grep -i "teensy\|usb" | awk '{print $$1}' | head -1)

.PHONY: all compile upload clean port

all: compile

compile:
	@echo "Copying source to $(SKETCH_DIR) (Arduino requires dir name = sketch name)..."
	@rm -rf $(SKETCH_DIR)
	@cp -r $(SRC_DIR) $(SKETCH_DIR)
	@echo "Compiling for Teensy 4.0..."
	arduino-cli compile --fqbn $(FQBN) $(SKETCH_DIR)

upload: compile
	@if [ -z "$(PORT)" ]; then \
		echo "ERROR: No Teensy found. Connect via USB and try again."; \
		echo "Available ports:"; \
		arduino-cli board list; \
		exit 1; \
	fi
	@echo "Flashing to $(PORT)..."
	arduino-cli upload --fqbn $(FQBN) --port $(PORT) $(SKETCH_DIR)

clean:
	rm -rf $(SKETCH_DIR)

port:
	arduino-cli board list
