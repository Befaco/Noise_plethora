// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"

class xModRingSqr : public Plugin {

public:

  xModRingSqr()
  :patchCord1(waveformMod2, 0, multiply1, 1),
  patchCord2(waveformMod2, 0, waveformMod1, 0),
  patchCord3(waveformMod1, 0, multiply1, 0),
  patchCord4(waveformMod1, 0, waveformMod2, 0)
  { }

  ~xModRingSqr() override {}

  xModRingSqr(const xModRingSqr&) = delete;
  xModRingSqr& operator=(const xModRingSqr&) = delete;

  void init() override {
    waveformMod1.begin(0.8, 500 , WAVEFORM_SQUARE);
    waveformMod2.begin(0.8, 500 , WAVEFORM_SQUARE);

    waveformMod1.frequencyModulation (1);
    waveformMod2.frequencyModulation (1);

  }

  void process(float k1, float k2) override {
    float knob_1 = k1;
    float knob_2 = k2;
    float pitch1 = pow(knob_1, 2);
    float pitch2 = pow(knob_2, 2);


    waveformMod1.frequency(100+(pitch1*5000));
    waveformMod2.frequency(20+(pitch2*1000));

  }

  AudioStream& getStream() override { return multiply1; }
  unsigned char getPort() override { return 0; }

private:
  AudioSynthWaveformModulated waveformMod2;   //xy=464,422
  AudioSynthWaveformModulated waveformMod1;   //xy=472,319
  AudioEffectMultiply      multiply1;      //xy=682,361
  AudioConnection          patchCord1;
  AudioConnection          patchCord2;
  AudioConnection          patchCord3;
  AudioConnection          patchCord4;

};

REGISTER_PLUGIN(xModRingSqr);
