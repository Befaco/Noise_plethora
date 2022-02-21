// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"

class satanWorkout : public Plugin {

public:

  satanWorkout()
  : patchCord1(pink1, pwm1)
  , patchCord2(pwm1, freeverb1)
  { }

  ~satanWorkout() override {}

  // delete copy constructors
  satanWorkout(const satanWorkout&) = delete;
  satanWorkout& operator=(const satanWorkout&) = delete;

  void init() override {

    pink1.amplitude(1);
    pwm1.amplitude(1);
    freeverb1.damping(-5);

  }

  void process(float k1, float k2) override {

    float knob_1 = k1;
    float knob_2 = k2;

    float pitch1 = pow(knob_1, 2);
    // float pitch2 = pow(knob_2, 2);

    pwm1.frequency(8+pitch1*6000);

    freeverb1.roomsize(0.001+knob_2*4);
  }

  AudioStream& getStream() override { return freeverb1; }
  unsigned char getPort() override { return 0; }

private:
  // GUItool: begin automatically generated code
  // GUItool: begin automatically generated code
  AudioSynthNoisePink      pink1;          //xy=927.6667137145996,1712.0000095367432
  AudioSynthWaveformPWM    pwm1;           //xy=1065.6666564941406,1795.666675567627
  AudioEffectFreeverb      freeverb1;      //xy=1152.6666564941406,1739.6666746139526
  AudioConnection          patchCord1;
  AudioConnection          patchCord2;


};

REGISTER_PLUGIN(satanWorkout); // this is important, so that we can include the plugin in a bank
