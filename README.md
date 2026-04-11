# Noise Plethora Firmware

Extended firmware for the [Befaco Noise Plethora](https://www.befaco.org/noise-plethora/) Eurorack module. Based on the original v1.5 firmware by Jeremy Bernstein / Befaco, with 30 new algorithms and a stereo mode.

## What's New

- **30 new algorithms** across 3 additional banks (D, E, F) — total: 60 programs in 6 banks
- **Stereo mode** — both generators run the same algorithm with timbral decorrelation
- **Makefile** for easy build and flash via `arduino-cli`

See [NOISE_PLETHORA_PLUGIN_GUIDE.md](NOISE_PLETHORA_PLUGIN_GUIDE.md) for detailed documentation of all 60 algorithms.  
See [NOISE_PLETHORA_CATEGORIES.md](NOISE_PLETHORA_CATEGORIES.md) for programs grouped by use case (Drone, Harsh, Percussive, Textural, Chaotic).  
See [NOISE_PLETHORA_STEREO_GUIDE.md](NOISE_PLETHORA_STEREO_GUIDE.md) for stereo mode documentation and patch ideas.

## Banks

| Bank | Name | Theme |
|------|------|-------|
| A | Textures | Cross-modulation, ring mod, granular *(original)* |
| B | HH Clusters | Additive/cluster synthesis, 6–16 oscillators *(original)* |
| C | Harsh & Wild | Bitcrushing, random walks, reverb abuse *(original)* |
| **D** | **Resonant Bodies** | Comb filters, physical modeling, resonators |
| **E** | **Chaos Machines** | Logistic maps, attractors, cellular automata |
| **F** | **Stochastic** | Random pulses, metallic noise, LFSR sequences |

## Stereo Mode

Double-click the encoder to toggle. Both generators run the same algorithm, with XB controlling stereo width and YB controlling stereo movement. Both display dots light up when active. Double-click again to exit.

## Build & Flash

Requires [arduino-cli](https://arduino-cli.github.io/) with the Teensy board package installed.

> **Tested on:** macOS Sequoia 15.4 (Darwin 25.4.0, Apple Silicon). Linux and Windows have not been tested — the Makefile and patch paths may need adjustment for those platforms.

### First-time setup

```bash
# Install arduino-cli
brew install arduino-cli

# Add Teensy board support
arduino-cli config add board_manager.additional_urls https://www.pjrc.com/teensy/package_teensy_index.json
arduino-cli core update-index
arduino-cli core install teensy:avr

# Install dependencies
arduino-cli lib install "SevSeg"
# ADC and QuadEncoder are included with the Teensy core
```

### Patch Teensy Audio Library and compile

```bash
make setup        # patch AudioStream files (required once, originals backed up)
make              # compile firmware
make upload       # compile + flash (connect Teensy via USB first)
make port         # show available serial ports
make unpatch      # restore original Teensy AudioStream files
make clean        # remove build artifacts
```

To specify a port manually: `make upload PORT=/dev/cu.usbmodem12345`

## Hardware

- **MCU**: Teensy 4.0 (ARM Cortex-M7, 600 MHz)
- **Audio**: SGTL5000 codec, I2S stereo output, 44.1 kHz / 16-bit
- **Controls**: Quadrature encoder, 4 potentiometers, 2 CV inputs (10Vpp), button
- **Display**: 2x 7-segment LED
- **Size**: 14HP Eurorack

## Memory Usage (60 programs)

| Resource | Used | Free |
|----------|------|------|
| FLASH | 329 KB | 1703 KB (83% free) |
| RAM1 | 331 KB | 193 KB |
| RAM2 | 65 KB | 459 KB |

## License

- Original firmware: [CC-BY-NC-SA 3.0](LICENSE.txt) — Befaco / Jeremy Bernstein, 2021
- New plugins (Banks D, E, F): CC-BY-SA v4
- Stereo mode and infrastructure: CC-BY-SA v4
