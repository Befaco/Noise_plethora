// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"

class IceRain : public Plugin {

public:

  IceRain()
    : patchCord1(waveformMod1, 0, mixer1, 0)
    , patchCord2(waveformMod1, freeverb1)
    , patchCord3(freeverb1, 0, mixer1, 1)
    { }

  ~IceRain() override {}

  // delete copy constructors
  IceRain(const IceRain&) = delete;
  IceRain& operator=(const IceRain&) = delete;

  void init() override {
    waveformMod1.begin(1.0, 5, WAVEFORM_SAMPLE_HOLD);
    waveformMod1.frequencyModulation(10);

    freeverb1.roomsize(0.8);
    freeverb1.damping(0.8);

    mixer1.gain(0, 0.3);
    mixer1.gain(1, 1.0);
  }

  void process(float k1, float k2) override {
    float freq = 0.5f + pow(k1, 2) * 200.0f;
    float roomsize = 0.3f + k2 * 0.7f;

    waveformMod1.frequency(freq);
    freeverb1.roomsize(roomsize);

    mixer1.gain(0, 0.3f * (1.0f - k2));
    mixer1.gain(1, k2 * 2.0f);
  }

  AudioStream& getStream() override {
    return mixer1;
  }
  unsigned char getPort() override {
    return 0;
  }

private:
  AudioSynthWaveformModulated waveformMod1;
  AudioEffectFreeverb      freeverb1;
  AudioMixer4              mixer1;
  AudioConnection          patchCord1;
  AudioConnection          patchCord2;
  AudioConnection          patchCord3;
};

REGISTER_PLUGIN(IceRain);
