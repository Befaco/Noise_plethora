// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"

class Atari : public Plugin {

public:

  Atari()
  :patchCord1(waveformMod1, 0, waveformMod2, 1),
   patchCord3(waveformMod2, 0, waveformMod1, 0)
  { }

  ~Atari() override {}

  Atari(const Atari&) = delete;
  Atari& operator=(const Atari&) = delete;

  void init() override {
    int masterVolume= 1;
    waveformMod1.begin(WAVEFORM_SQUARE);
    waveformMod2.begin(WAVEFORM_PULSE);
    waveformMod1.offset(1);
    waveformMod1.amplitude(masterVolume);
    waveformMod2.amplitude(masterVolume);

  }

  void process(float k1, float k2) override {
    float knob_1 = k1;
    float knob_2 = k2;
    float pitch1 = pow(knob_1, 2);
    // float pitch2 = pow(knob_2, 2);
    waveformMod1.frequency(10+(pitch1*50));
    waveformMod2.frequency(10+(knob_2*200));
    waveformMod1.frequencyModulation(knob_2*8+3);

  }

  AudioStream& getStream() override { return waveformMod2; }
  unsigned char getPort() override { return 0; }

private:

  AudioSynthWaveformModulated waveformMod1;   //xy=334,349
  AudioSynthWaveformModulated waveformMod2; //xy=616,284
  AudioConnection          patchCord1;
  AudioConnection          patchCord3;

};

REGISTER_PLUGIN(Atari);
