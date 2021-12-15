// Noise Plethora
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported
// See LICENSE.txt for the complete license text

#pragma once

#include "Plugin.hpp"

class TriFMcluster : public Plugin {

public:

  TriFMcluster()
  :patchCord1(modulator1, 0, waveform1, 0),
  patchCord2(modulator3, 0, waveform3, 0),
  patchCord3(modulator2, 0, waveform2, 0),
  patchCord4(modulator5, 0, waveform5, 0),
  patchCord5(modulator4, 0, waveform4, 0),
  patchCord6(modulator6, 0, waveform6, 0),
  patchCord7(waveform1, 0, mixer1, 0),
  patchCord8(waveform4, 0, mixer1, 3),
  patchCord9(waveform2, 0, mixer1, 1),
  patchCord10(waveform3, 0, mixer1, 2),
  patchCord11(waveform6, 0, mixer2, 1),
  patchCord12(waveform5, 0, mixer2, 0),
  patchCord13(mixer2, 0, mixer5, 1),
  patchCord14(mixer1, 0, mixer5, 0)
  { }

  ~TriFMcluster() override {}

  TriFMcluster(const TriFMcluster&) = delete;
  TriFMcluster& operator=(const TriFMcluster&) = delete;

  void init() override {
    mixer1.gain(0,1);
    mixer1.gain(1,1);
    mixer1.gain(2,1);
    mixer1.gain(3,1);
    mixer2.gain(0,1);
    mixer2.gain(1,1);
    mixer2.gain(2,1);
    mixer2.gain(3,1);
    mixer5.gain(0,1);
    mixer5.gain(1,1);
    mixer5.gain(2,1);
    mixer5.gain(3,1);

    int masterWaveform= WAVEFORM_TRIANGLE;
    float masterVolume=0.25;

    waveform1.begin(masterVolume, 794, masterWaveform);

    waveform2.begin(masterVolume, 647, masterWaveform);

    waveform3.begin(masterVolume, 524, masterWaveform);

    waveform4.begin(masterVolume, 444, masterWaveform);

    waveform5.begin(masterVolume, 368, masterWaveform);

    waveform6.begin(masterVolume, 283, masterWaveform);

    modulator1.begin(1, 1000, WAVEFORM_SINE);
    modulator2.begin(1, 1000, WAVEFORM_SINE);
    modulator3.begin(1, 1000, WAVEFORM_SINE);
    modulator4.begin(1, 1000, WAVEFORM_SINE);
    modulator5.begin(1, 1000, WAVEFORM_SINE);
    modulator6.begin(1, 1000, WAVEFORM_SINE);

  }

  void process(float k1, float k2) override {
    float knob_1 = k1;
    float knob_2 = k2;

    float pitch1 = pow(knob_1, 2);



    float f1 = 300+pitch1*8000;
    float f2 = f1*1.227;
    float f3 = f2*1.24;
    float f4 = f3*1.17;
    float f5 = f4*1.2;
    float f6 = f5*1.3;


    float index = knob_2*0.9+0.1;
    float indexFreq = 0.07;


    modulator1.amplitude(index);
    modulator2.amplitude(index);
    modulator3.amplitude(index);
    modulator4.amplitude(index);
    modulator5.amplitude(index);
    modulator6.amplitude(index);


    modulator1.frequency(f1*indexFreq);
    modulator2.frequency(f2*indexFreq);
    modulator3.frequency(f3*indexFreq);
    modulator4.frequency(f4*indexFreq);
    modulator5.frequency(f5*indexFreq);
    modulator6.frequency(f6*indexFreq);


    waveform1.frequency(f1);
    waveform2.frequency(f2);
    waveform3.frequency(f3);
    waveform4.frequency(f4);
    waveform5.frequency(f5);
    waveform6.frequency(f6);

  }

  AudioStream& getStream() override { return mixer5; }
  unsigned char getPort() override { return 0; }

private:
  AudioSynthWaveform       modulator1;      //xy=236.88888549804688,262.55556869506836
  AudioSynthWaveform       modulator3; //xy=238.88890075683594,366.555606842041
  AudioSynthWaveform       modulator2; //xy=239.88890075683594,313.5556392669678
  AudioSynthWaveform       modulator5; //xy=239.88890075683594,485.5555810928345
  AudioSynthWaveform       modulator4; //xy=240.88890075683594,428.55560970306396
  AudioSynthWaveform       modulator6; //xy=242.8888931274414,541.5555143356323
  AudioSynthWaveformModulated waveform1;      //xy=459.88890075683594,268.6667013168335
  AudioSynthWaveformModulated waveform4;      //xy=459.8888854980469,433.6666889190674
  AudioSynthWaveformModulated waveform2;      //xy=461.88890838623047,322.66671657562256
  AudioSynthWaveformModulated waveform3;      //xy=461.8888854980469,373.6666889190674
  AudioSynthWaveformModulated waveform6;      //xy=461.88890838623047,546.6666688919067
  AudioSynthWaveformModulated waveform5;      //xy=462.88888931274414,492.66670417785645
  AudioMixer4              mixer2;         //xy=663.8888053894043,520.6666412353516
  AudioMixer4              mixer1;         //xy=667.888916015625,349.6667251586914
  AudioMixer4              mixer5;         //xy=853.8889122009277,448.6667900085449
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
  AudioConnection          patchCord13;
  AudioConnection          patchCord14;

};

REGISTER_PLUGIN(TriFMcluster);
