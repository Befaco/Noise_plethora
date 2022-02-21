// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"

class delayPonzo : public Plugin {

public:

  delayPonzo()
  :patchCord1(waveformMod2, 0, mixer1, 1),
   patchCord2(waveformMod1, 0, mixer1, 0),
   patchCord3(mixer1, delay1),
   patchCord4(delay1, 0, waveformMod1, 0),
   patchCord6(delay1, 1, waveformMod2, 0)
  { }

  ~delayPonzo() override {}

  delayPonzo(const delayPonzo&) = delete;
  delayPonzo& operator=(const delayPonzo&) = delete;

  void init() override {

    int masterVolume= 1;


    waveformMod1.begin(WAVEFORM_SQUARE);
    waveformMod2.begin(WAVEFORM_PULSE);
    waveformMod1.amplitude(masterVolume);
    waveformMod2.amplitude(masterVolume);

  }

  void process(float k1, float k2) override {
    float knob_1 = k1;
    float knob_2 = k2;
    float pitch1 = pow(knob_1, 2);


  waveformMod1.frequency(10+(pitch1*50));
  waveformMod2.frequency(10+(pitch1*1500));

  //waveformMod2.frequency(10+(knob_2*200));

  delay1.delay(0, knob_2*100+10);
  delay1.delay(1, knob_2*250+10);

  mixer1.gain(0,1);
  mixer1.gain(1,2);

  waveformMod1.frequencyModulation(4);
  waveformMod2.frequencyModulation(2);


  }

  AudioStream& getStream() override { return delay1; }
  unsigned char getPort() override { return 0; }

private:
  AudioSynthWaveformModulated waveformMod2;   //xy=553.75,785.7500448226929
  AudioSynthWaveformModulated waveformMod1;   //xy=590.75,318.75002574920654
  AudioMixer4              mixer1;         //xy=600.75,521.75
  AudioEffectDelay         delay1;         //xy=796.7500152587891,736.7500305175781
  AudioConnection          patchCord1;
  AudioConnection          patchCord2;
  AudioConnection          patchCord3;
  AudioConnection          patchCord4;
  AudioConnection          patchCord6;
};

REGISTER_PLUGIN(delayPonzo);
