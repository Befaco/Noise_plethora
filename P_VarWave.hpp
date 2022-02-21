// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"



class VarWave: public Plugin {
public:

  VarWave()
    : patchCord1(waveform1, 0, waveformMod1, 0)
  {}
  ~VarWave() override {}

  // delete copy constructors
  VarWave(const VarWave&) = delete;
  VarWave& operator=(const VarWave&) = delete;


void init() override {

  //noise1.amplitude(1);
  waveformMod1.arbitraryWaveform(myWaveform, 172.0);

  waveformMod1.begin(1, 250, WAVEFORM_ARBITRARY);
  waveform1.begin(1, 500, WAVEFORM_SINE);




}

void process(float k1, float k2) override {
  float knob_1 = k1;
  float knob_2 = k2;

  float pitch1 = pow(knob_1, 2);
  // float pitch2 = pow(knob_2, 2);
  for(int i=0;i<256;i++){
    test=random(-28000, 28000);

    myWaveform[i] = test;
  }


  waveformMod1.frequency(10+(pitch1*10000));
  waveform1.frequency(100+(pitch1*500));


  waveform1.amplitude(knob_2);


}
AudioStream& getStream() override { return waveformMod1; }
unsigned char getPort() override { return 0; }
private:

  // GUItool: begin automatically generated code
  AudioSynthWaveform       waveform1;      //xy=305.3333282470703,648.3333358764648
  AudioSynthWaveformModulated waveformMod1;   //xy=475.88893127441406,517.2221565246582
  AudioConnection          patchCord1;

  int16_t test;
  int16_t myWaveform[256];

  int current_waveform=0;

              /* PREGUNTA A DIEGO ¿Este sobra? */
  //extern const int16_t myWaveform[256];  // defined in myWaveform.ino


};






REGISTER_PLUGIN(VarWave); // this is important, so that we can include the plugin in a bank
