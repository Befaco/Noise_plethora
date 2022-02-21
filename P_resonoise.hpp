// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"

class resonoise : public Plugin {

public:

  resonoise()
  :patchCord1(waveformMod1, sine_fm1)
  , patchCord2(noise1, 0, filter1, 0)
  , patchCord3(sine_fm1, 0, wavefolder1, 0)
  , patchCord4(dc1, 0, wavefolder1, 1)
  , patchCord5(wavefolder1, 0, filter1, 1)
  { }

  ~resonoise() override {}

  // delete copy constructors
  resonoise(const resonoise&) = delete;
  resonoise& operator=(const resonoise&) = delete;

  void init() override {

    noise1.amplitude(1);

    sine_fm1.frequency(1100);


    sine_fm1.amplitude(1);
    waveformMod1.begin(1, 500, WAVEFORM_SQUARE);

    filter1.resonance(3);
    filter1.octaveControl(5);
  }

  void process(float k1, float k2) override {


    float knob_1 = k1;
    float knob_2 = k2;

    float pitch1 = pow(knob_1, 2);

    dc1.amplitude(knob_2*0.2+0.03, 1);

    sine_fm1.frequency(20+(pitch1*10000));
    waveformMod1.frequency(20+(pitch1*7777));


    /*Serial.print(0.2+(knob_2*3));
    Serial.println();*/


  }
  AudioStream& getStream() override { return filter1; }
  unsigned char getPort() override { return 0; }

private:
  // GUItool: begin automatically generated code
  AudioSynthWaveformModulated waveformMod1;   //xy=245.88888549804688,699.2222213745117
  AudioSynthNoiseWhite     noise1;         //xy=271.8888854980469,354.8888854980469
  AudioSynthWaveformSineModulated sine_fm1;       //xy=303.88890838623047,455.8889751434326
  AudioSynthWaveformDc     dc1;            //xy=436.8888854980469,646.1111145019531
  AudioEffectWaveFolder    wavefolder1;    //xy=469.8888854980469,520.1111145019531
  AudioFilterStateVariable filter1;        //xy=559.8888854980469,410.88891983032227
  AudioConnection          patchCord1;
  AudioConnection          patchCord2;
  AudioConnection          patchCord3;
  AudioConnection          patchCord4;
  AudioConnection          patchCord5;

};

REGISTER_PLUGIN(resonoise); // this is important, so that we can include the plugin in a bank
