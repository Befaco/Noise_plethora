// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"

class MetallicNoise : public Plugin {

public:

  MetallicNoise()
    : patchCord1(osc[0], 0, mixer1, 0)
    , patchCord2(osc[1], 0, mixer1, 1)
    , patchCord3(osc[2], 0, mixer1, 2)
    , patchCord4(osc[3], 0, mixer1, 3)
    , patchCord5(osc[4], 0, mixer2, 0)
    , patchCord6(osc[5], 0, mixer2, 1)
    , patchCord7(mixer1, 0, mixer3, 0)
    , patchCord8(mixer2, 0, mixer3, 1)
    , patchCord9(mixer3, 0, filter1, 0)
    { }

  ~MetallicNoise() override {}

  MetallicNoise(const MetallicNoise&) = delete;
  MetallicNoise& operator=(const MetallicNoise&) = delete;

  void init() override {
    for (int i = 0; i < 6; i++) {
      osc[i].begin(0.15f, baseFreqs[i], WAVEFORM_SQUARE);  // 6*0.15=0.9 max sum, no clipping
    }

    mixer1.gain(0, 1.0f);
    mixer1.gain(1, 1.0f);
    mixer1.gain(2, 1.0f);
    mixer1.gain(3, 1.0f);

    mixer2.gain(0, 1.0f);
    mixer2.gain(1, 1.0f);
    mixer2.gain(2, 0.0f);
    mixer2.gain(3, 0.0f);

    mixer3.gain(0, 1.0f);
    mixer3.gain(1, 1.0f);
    mixer3.gain(2, 0.0f);
    mixer3.gain(3, 0.0f);

    filter1.frequency(5000);
    filter1.resonance(0.7f);
    filter1.octaveControl(2.0f);
  }

  void process(float k1, float k2) override {
    float pitchMult = 0.5f + k1 * 2.0f;
    for (int i = 0; i < 6; i++) {
      osc[i].frequency(baseFreqs[i] * pitchMult);
    }

    float filterFreq = 500.0f + pow(k2, 2) * 15000.0f;
    filter1.frequency(filterFreq);
  }

  AudioStream& getStream() override {
    return filter1;
  }
  unsigned char getPort() override {
    return 2;
  }

private:
  static constexpr float baseFreqs[6] = {205.3f, 304.4f, 369.6f, 522.7f, 800.6f, 1053.4f};

  AudioSynthWaveformModulated osc[6];
  AudioMixer4              mixer1;
  AudioMixer4              mixer2;
  AudioMixer4              mixer3;
  AudioFilterStateVariable filter1;
  AudioConnection          patchCord1;
  AudioConnection          patchCord2;
  AudioConnection          patchCord3;
  AudioConnection          patchCord4;
  AudioConnection          patchCord5;
  AudioConnection          patchCord6;
  AudioConnection          patchCord7;
  AudioConnection          patchCord8;
  AudioConnection          patchCord9;
};

REGISTER_PLUGIN(MetallicNoise);
