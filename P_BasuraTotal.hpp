// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"

class BasuraTotal : public Plugin {

public:

  BasuraTotal()
    : patchCord1(waveformMod1, freeverb1)
  {}

  ~BasuraTotal() override {}

  BasuraTotal(const BasuraTotal&) = delete;
  BasuraTotal& operator=(const BasuraTotal&) = delete;

  void init() override {
    freeverb1.roomsize(0);
    lastClick = micros();
    waveformMod1.begin(1, 500, WAVEFORM_SINE);
  }

  void process(float k1, float k2) override {
    float knob_1 = k1;
    float knob_2 = k2;

    float pitch1 = pow(knob_1, 2);
    float pitch2 = pow(knob_2, 2);

    if ((micros() - lastClick) > 100000 * pitch2) { // Changing this value changes the frequency.
      lastClick = micros();
      waveformMod1.begin(1, 500, WAVEFORM_SQUARE);
      waveformMod1.frequency(generateNoise() * (200 + (pitch1 * 5000))) ;
      freeverb1.roomsize(1);
    }
  }

  AudioStream& getStream() override { return freeverb1; }
  unsigned char getPort() override { return 0; }

private:

  static unsigned int generateNoise() {
    // See https://en.wikipedia.org/wiki/Linear_feedback_shift_register#Galois_LFSRs
    /* initialize with any 32 bit non-zero  unsigned long value. */
    static unsigned long int lfsr = 0xfeddfaceUL; /* 32 bit init, nonzero */
    static unsigned long mask = ((unsigned long)( 1UL<<31 | 1UL <<15 | 1UL <<2 | 1UL <<1 ));
    /* If the output bit is 1, apply toggle mask.
     * The value has 1 at bits corresponding
     * to taps, 0 elsewhere. */

    if (lfsr & 1) {
      lfsr = (lfsr >>1) ^ mask;
      return(1);
    }
    else {
      lfsr >>= 1;
      return(0);
    }
  }

  AudioSynthWaveformModulated waveformMod1;   //xy=216.88888549804688,217.9999988898635
  AudioEffectFreeverb         freeverb1;      //xy=374.8888854980469,153.88888549804688
  AudioConnection             patchCord1;
  unsigned long               lastClick;
};

REGISTER_PLUGIN(BasuraTotal);

