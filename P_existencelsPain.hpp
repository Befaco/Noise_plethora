// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"

class existencelsPain : public Plugin {

public:

  existencelsPain()
  : patchCord1(waveform1, 0, filter1, 0)
  , patchCord2(waveform1, 0, filter2, 0)
  , patchCord3(waveform1, 0, filter3, 0)
  , patchCord4(waveform1, 0, filter4, 0)
  , patchCord5(waveformMod1, 0, filter1, 1)
  , patchCord6(waveformMod2, 0, filter2, 1)
  , patchCord7(waveformMod4, 0, filter4, 1)
  , patchCord8(waveformMod3, 0, filter3, 1)
  , patchCord9(filter1, 1, mixer1, 0)
  , patchCord10(filter2, 1, mixer1, 1)
  , patchCord11(filter3, 1, mixer1, 2)
  , patchCord12(filter4, 1, mixer1, 3)
  { }

  ~existencelsPain() override {}

  // delete copy constructors
  existencelsPain(const existencelsPain&) = delete;
  existencelsPain& operator=(const existencelsPain&) = delete;

  void init() override {

    //noise1.amplitude(1);

    mixer1.gain(0,0.8);
    mixer1.gain(1,0.8);
    mixer1.gain(2,0.8);
    mixer1.gain(3,0.8);


    waveformMod1.begin(1, 11, WAVEFORM_TRIANGLE);
    waveformMod2.begin(1, 70, WAVEFORM_TRIANGLE);
    waveformMod3.begin(1, 23, WAVEFORM_TRIANGLE);
    waveformMod4.begin(1, 0.01, WAVEFORM_TRIANGLE);

    waveform1.begin(1, 5,WAVEFORM_SAMPLE_HOLD );
    //waveform1.pulseWidth(0.1);

    filter1.resonance(5);
    filter1.octaveControl(7);

    filter2.resonance(5);
    filter2.octaveControl(7);

    filter3.resonance(5);
    filter3.octaveControl(7);

    filter4.resonance(5);
    filter4.octaveControl(7);
  }

  void process(float k1, float k2) override {


    float knob_1 = k1;
    float knob_2 = k2;

    float pitch1 = pow(knob_1, 2);
    // float pitch2 = pow(knob_2, 2);


    waveform1.frequency(50+(pitch1*5000));

    float octaves=knob_2*3+0.3;

    /*filter1.resonance(resonanceLevel);
    filter2.resonance(resonanceLevel);
    filter3.resonance(resonanceLevel);
    filter4.resonance(resonanceLevel);*/

    filter1.octaveControl(octaves);
    filter2.octaveControl(octaves);
    filter3.octaveControl(octaves);
    filter4.octaveControl(octaves);
}
AudioStream& getStream() override { return mixer1; }
unsigned char getPort() override { return 0; }

private:
  // GUItool: begin automatically generated code
  AudioSynthWaveform       waveform1;      //xy=283,566
  AudioSynthWaveformModulated waveformMod1;   //xy=363.888916015625,321.2221794128418
  AudioSynthWaveformModulated waveformMod2; //xy=368,380.3332633972168
  AudioSynthWaveformModulated waveformMod4; //xy=374,689.3332633972168
  AudioSynthWaveformModulated waveformMod3; //xy=378,638.3332633972168
  AudioFilterStateVariable filter1;        //xy=578.888916015625,420.8889274597168
  AudioFilterStateVariable filter2; //xy=579,486.0000114440918
  AudioFilterStateVariable filter3; //xy=581,549.0000114440918
  AudioFilterStateVariable filter4; //xy=581,614.0000114440918
  AudioMixer4              mixer1;         //xy=752,520.0000114440918
  AudioConnection          patchCord1;
  AudioConnection          patchCord2;
  AudioConnection          patchCord3;
  AudioConnection          patchCord4;
  AudioConnection          patchCord5;
  AudioConnection          patchCord6;
  AudioConnection          patchCord7;
  AudioConnection          patchCord8;
  AudioConnection          patchCord9;
  AudioConnection          patchCord10;
  AudioConnection          patchCord11;
  AudioConnection          patchCord12;

};

REGISTER_PLUGIN(existencelsPain); // this is important, so that we can include the plugin in a bank
