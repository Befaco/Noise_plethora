// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"

class NoiseHarmonics : public Plugin {

public:

  NoiseHarmonics()
    : patchCord1(noise1, 0, wavefolder1, 0)
    , patchCord2(dc1, 0, wavefolder1, 1)
    , patchCord3(wavefolder1, 0, filter1, 0)
    { }

  ~NoiseHarmonics() override {}

  // delete copy constructors
  NoiseHarmonics(const NoiseHarmonics&) = delete;
  NoiseHarmonics& operator=(const NoiseHarmonics&) = delete;

  void init() override {
    noise1.amplitude(1.0);
    dc1.amplitude(0.5);

    filter1.frequency(1000);
    filter1.resonance(1.5);       // lower Q = wider, grittier character
    filter1.octaveControl(2.0);
  }

  void process(float k1, float k2) override {
    float dcAmp = 0.1f + pow(k1, 2) * 1.5f;  // wider range, more aggressive folding
    float filterFreq = 60.0f + pow(k2, 2) * 8000.0f;

    dc1.amplitude(dcAmp);
    filter1.frequency(filterFreq);
  }

  AudioStream& getStream() override {
    return filter1;
  }
  unsigned char getPort() override {
    return 0;
  }

private:
  AudioSynthNoiseWhite     noise1;
  AudioSynthWaveformDc     dc1;
  AudioEffectWaveFolder    wavefolder1;
  AudioFilterStateVariable filter1;
  AudioConnection          patchCord1;
  AudioConnection          patchCord2;
  AudioConnection          patchCord3;
};

REGISTER_PLUGIN(NoiseHarmonics);
