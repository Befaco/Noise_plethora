// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under Creative Commons CC-BY-SA v4text

#pragma once

#include "Plugin.hpp"



class Rwalk_WaveTwist: public Plugin {
public:

  Rwalk_WaveTwist()
  : patchCord1(waveform1, 0, waveformMod1, 0)
  {}
    ~Rwalk_WaveTwist() override {}

    // delete copy constructors
    Rwalk_WaveTwist(const Rwalk_WaveTwist&) = delete;
    Rwalk_WaveTwist& operator=(const Rwalk_WaveTwist&) = delete;


    void init() override {

      L = 20000; // Size of box: maximum frequency
      v_0 = 10; // speed: size of step in frequency units.

      //noise1.amplitude(1);

      waveform1.begin(1, 200, WAVEFORM_SAWTOOTH);
      waveformMod1.begin(1, 250, WAVEFORM_ARBITRARY);

      waveformMod1.arbitraryWaveform(myWaveform, 172.0);

      // random walk initial conditions
      for(int i=0; i<256; i++)
      {
        // velocities: initial conditions in -pi : +pi
        theta = PI*(random(2.0)-1.0);
        vx[i] = cos(theta);
        vy[i] = sin(theta);
        // positions: random in [0,L] x [0, L]
        x[i] = random(-L, L);
        y[i] = random(-L, L);

      }
    }

    void process(float k1, float k2) override {
      float knob_1 = k1;
      float knob_2 = k2;

      float pitch1 = pow(knob_1, 2);
      // float pitch2 = pow(knob_2, 2);


      float dL = L;

      v_var = v_0;


      // loop to "walk" randomly
      for(int i=0; i<256; i++)
      {
        theta = PI*(random(2.0)-1.0);

        posx = cos(theta);
        vx[i] = posx;

        posy = sin(theta);
        vy[i] = posy;

        // Update new position
        xn = x[i] + v_var * vx[i];
        yin = y[i] + v_var * vy[i];

        // periodic boundary conditions
        if(xn < -dL+100)
        //xn += dL;
        xn += 100;
        else if (xn > dL)
        //xn -= dL;
        xn -= 100;

        if(yin < 0.01)
        yin += dL;
        else if (yin > dL)
        yin -= dL;
        x[i] = xn;
        y[i] = yin;
        if(i < 30){
          myWaveform[i] = xn;
        } else{
          myWaveform[i] = -100;
        }

      }
      waveformMod1.arbitraryWaveform(myWaveform, 172.0);

      waveform1.frequency(10 + (pitch1*100));
      waveformMod1.amplitude(knob_2);



    }
    AudioStream& getStream() override { return waveformMod1; }
    unsigned char getPort() override { return 0; }
  private:

    // GUItool: begin automatically generated code
    AudioSynthWaveformModulated waveformMod1;   //xy=475.88893127441406,517.2221565246582
    AudioSynthWaveform       waveform1;      //xy=673,474
    AudioConnection          patchCord1;
    int16_t test;
    int16_t myWaveform[256];

    int current_waveform=0;

    int L, i, t;
    float theta, posx, posy, xn, yin;
    float v_0, v_var;//pw = pulse width
    float x[256], y[256], vx[256], vy[256]; // number depends on waveforms declared


  };



  REGISTER_PLUGIN(Rwalk_WaveTwist); // this is important, so that we can include the plugin in a bank
