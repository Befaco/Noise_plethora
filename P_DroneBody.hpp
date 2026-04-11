// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"

class DroneBody : public Plugin {

public:

  DroneBody()
    : patchCord1(sine_fm2, 0, sine_fm1, 0)
    , patchCord2(sine_fm1, 0, sine_fm2, 0)
    , patchCord3(sine_fm1, 0, mixer1, 0)
    , patchCord4(sine_fm2, 0, mixer1, 1)
    , patchCord5(mixer1, 0, wavefolder1, 0)
    , patchCord6(dc1, 0, wavefolder1, 1)
    { }

  ~DroneBody() override {}

  // delete copy constructors
  DroneBody(const DroneBody&) = delete;
  DroneBody& operator=(const DroneBody&) = delete;

  void init() override {
    sine_fm1.frequency(100);
    sine_fm1.amplitude(1.0);

    sine_fm2.frequency(101);
    sine_fm2.amplitude(1.0);

    dc1.amplitude(0.3);

    mixer1.gain(0, 0.5);
    mixer1.gain(1, 0.5);
  }

  void process(float k1, float k2) override {
    float freq = 20.0f + pow(k1, 2) * 500.0f;
    float detune_ratio = 1.0f + k2 * 0.05f;
    float dc_fold = 0.1f + k2 * 0.5f;

    sine_fm1.frequency(freq);
    sine_fm2.frequency(freq * detune_ratio);
    dc1.amplitude(dc_fold);
  }

  AudioStream& getStream() override {
    return wavefolder1;
  }
  unsigned char getPort() override {
    return 0;
  }

private:
  AudioSynthWaveformSineModulated sine_fm1;
  AudioSynthWaveformSineModulated sine_fm2;
  AudioSynthWaveformDc     dc1;
  AudioEffectWaveFolder    wavefolder1;
  AudioMixer4              mixer1;
  AudioConnection          patchCord1;
  AudioConnection          patchCord2;
  AudioConnection          patchCord3;
  AudioConnection          patchCord4;
  AudioConnection          patchCord5;
  AudioConnection          patchCord6;
};

REGISTER_PLUGIN(DroneBody);
