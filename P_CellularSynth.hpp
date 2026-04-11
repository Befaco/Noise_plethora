// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"
#include "AudioStreamCustom.hpp"

class CellularSynth : public Plugin {

public:

  CellularSynth() {}

  ~CellularSynth() override {}

  CellularSynth(const CellularSynth&) = delete;
  CellularSynth& operator=(const CellularSynth&) = delete;

  void init() override {
    // Initialize CA with single cell in center
    for (int i = 0; i < 32; i++) {
      cells[i] = false;
    }
    cells[16] = true;

    clockCounter = 0;
    clockSamples = 128;
    rule = 110;

    float fundamental = 55.0f;

    for (int i = 0; i < 8; i++) {
      customStream.oscPhase[i] = 0.0f;
      customStream.oscFreq[i] = fundamental * (i + 1);
      customStream.oscAmp[i] = 0.2f;
    }

    // Set initial amplitudes from CA state
    updateOscAmplitudes();
  }

  void process(float k1, float k2) override {
    rule = (int)(k1 * 255.0f);
    clockSamples = (int)(128 + (1.0f - k2) * 128.0f * 50.0f);

    clockCounter += AUDIO_BLOCK_SAMPLES;
    if (clockCounter >= clockSamples) {
      evolveCA();
      updateOscAmplitudes();
      clockCounter = 0;
    }
  }

  AudioStream& getStream() override { return customStream; }
  unsigned char getPort() override { return 0; }

private:

  void evolveCA() {
    bool newCells[32];
    for (int i = 0; i < 32; i++) {
      int left = (i - 1 + 32) % 32;
      int right = (i + 1) % 32;

      int pattern = (cells[left] ? 4 : 0)
                  | (cells[i]    ? 2 : 0)
                  | (cells[right] ? 1 : 0);

      newCells[i] = ((rule >> pattern) & 1) != 0;
    }
    for (int i = 0; i < 32; i++) {
      cells[i] = newCells[i];
    }
  }

  void updateOscAmplitudes() {
    // 32 cells grouped into 8 groups of 4
    for (int g = 0; g < 8; g++) {
      int count = 0;
      for (int c = 0; c < 4; c++) {
        if (cells[g * 4 + c]) count++;
      }
      float amp = count * 0.25f; // 0, 0.25, 0.5, 0.75, 1.0
      customStream.oscAmp[g] = amp * 0.2f; // master volume scaling
    }
  }

  bool cells[32] = {};
  int clockCounter = 0;
  int clockSamples = 128;
  int rule = 110;

  class Stream : public AudioStreamCustom {
  public:
    float oscPhase[8] = {};
    float oscFreq[8] = {};
    float oscAmp[8] = {};

    void fillBlock(audio_block_t *block) override {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        float mixed = 0.0f;

        for (int o = 0; o < 8; o++) {
          // Sawtooth oscillator: phase accumulator
          oscPhase[o] += oscFreq[o] / 44100.0f;
          if (oscPhase[o] >= 1.0f) oscPhase[o] -= 1.0f;

          // Sawtooth: -1 to +1
          float saw = oscPhase[o] * 2.0f - 1.0f;
          mixed += saw * oscAmp[o];
        }

        // Master mix scaling (equivalent to mixer gains 0.7 * 0.5)
        mixed *= 0.7f;

        if (mixed > 1.0f) mixed = 1.0f;
        if (mixed < -1.0f) mixed = -1.0f;
        block->data[i] = (int16_t)(mixed * 32767.0f);
      }
    }
  } customStream;
};

REGISTER_PLUGIN(CellularSynth);
