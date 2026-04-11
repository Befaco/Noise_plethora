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
TEENSY_CORE := $(shell find $(HOME)/Library/Arduino15/packages/teensy/hardware/avr -maxdepth 1 -type d 2>/dev/null | tail -1)

.PHONY: all compile upload clean port patch setup

all: compile

setup: patch
	@echo "Setup complete. Run 'make' to compile."

patch:
	@if [ -z "$(TEENSY_CORE)" ]; then \
		echo "ERROR: Teensy core not found. Run first:"; \
		echo "  arduino-cli config add board_manager.additional_urls https://www.pjrc.com/teensy/package_teensy_index.json"; \
		echo "  arduino-cli core update-index && arduino-cli core install teensy:avr"; \
		exit 1; \
	fi
	@echo "Patching Teensy AudioStream at $(TEENSY_CORE)..."
	@cp "$(TEENSY_CORE)/cores/teensy4/AudioStream.h" "$(TEENSY_CORE)/cores/teensy4/AudioStream.h.bak" 2>/dev/null || true
	@cp "$(TEENSY_CORE)/cores/teensy4/AudioStream.cpp" "$(TEENSY_CORE)/cores/teensy4/AudioStream.cpp.bak" 2>/dev/null || true
	@cp teensy/avr/cores/teensy4/AudioStream.h "$(TEENSY_CORE)/cores/teensy4/"
	@cp teensy/avr/cores/teensy4/AudioStream.cpp "$(TEENSY_CORE)/cores/teensy4/"
	@DELAY=$$(find "$(TEENSY_CORE)" -name "effect_delay.h" -path "*/Audio/*" | head -1); \
	if [ -n "$$DELAY" ]; then \
		cp "$$DELAY" "$${DELAY}.bak" 2>/dev/null || true; \
		cp teensy/avr/libraries/Audio/effect_delay.h "$$DELAY"; \
		echo "Patched: AudioStream.h, AudioStream.cpp, effect_delay.h"; \
	else \
		echo "Patched: AudioStream.h, AudioStream.cpp (effect_delay.h not found)"; \
	fi
	@echo "Originals backed up with .bak extension."

unpatch:
	@if [ -z "$(TEENSY_CORE)" ]; then echo "Teensy core not found."; exit 1; fi
	@echo "Restoring original Teensy AudioStream..."
	@cd "$(TEENSY_CORE)/cores/teensy4" && \
		[ -f AudioStream.h.bak ] && mv AudioStream.h.bak AudioStream.h && \
		[ -f AudioStream.cpp.bak ] && mv AudioStream.cpp.bak AudioStream.cpp || true
	@DELAY=$$(find "$(TEENSY_CORE)" -name "effect_delay.h.bak" -path "*/Audio/*" | head -1); \
	if [ -n "$$DELAY" ]; then mv "$$DELAY" "$${DELAY%.bak}"; fi
	@echo "Originals restored."

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
