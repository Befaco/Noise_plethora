// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"

class BitShift : public Plugin {

public:

  BitShift()
    : patchCord1(waveform1, 0, combine1, 0)
    , patchCord2(waveform2, 0, combine1, 1)
    { }

  ~BitShift() override {}

  BitShift(const BitShift&) = delete;
  BitShift& operator=(const BitShift&) = delete;

  void init() override {
    // sawtooths produce diverse bit patterns -- XOR creates rich digital artifacts
    // (square waves only have 2 values, XOR of +/-32767 produces near-silence)
    waveform1.begin(1.0f, 100, WAVEFORM_SAWTOOTH);
    waveform2.begin(1.0f, 200, WAVEFORM_SAWTOOTH);
    combine1.setCombineMode(1); // XOR
  }

  void process(float k1, float k2) override {
    float freq1 = 20.0f + pow(k1, 2) * 5000.0f;
    float freq2 = freq1 * (1.0f + k2 * 7.0f);
    waveform1.frequency(freq1);
    waveform2.frequency(freq2);
  }

  AudioStream& getStream() override {
    return combine1;
  }
  unsigned char getPort() override {
    return 0;
  }

private:
  AudioSynthWaveformModulated waveform1;
  AudioSynthWaveformModulated waveform2;
  AudioEffectDigitalCombine combine1;
  AudioConnection          patchCord1;
  AudioConnection          patchCord2;
};

REGISTER_PLUGIN(BitShift);
