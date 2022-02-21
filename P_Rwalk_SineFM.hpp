// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Julia Mugica / Ivan Paz
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"

class Rwalk_SineFM : public Plugin {

  public:

    Rwalk_SineFM()
    : patchCord1(waveform1, sine_fm1)
    , patchCord2(waveform2, sine_fm2)
    , patchCord3(sine_fm1, 0, mixer1, 1)
    , patchCord4(sine_fm2, 0, mixer1, 2)
    { }

    ~Rwalk_SineFM() override {}

    // delete copy constructors
    Rwalk_SineFM(const Rwalk_SineFM&) = delete;
    Rwalk_SineFM& operator=(const Rwalk_SineFM&) = delete;

    void init() override {


      L = 600; // Size of box: maximum frequency
      v_0 = 30; // speed: size of step in frequency units.

      mixer1.gain(0,1);
      mixer1.gain(2,1);

      sine_fm1.amplitude(1);
      sine_fm2.amplitude(1);

      int masterWaveform= WAVEFORM_PULSE;


      waveform1.pulseWidth(0.2);
      waveform1.begin(1, 794, masterWaveform);

      waveform2.pulseWidth(0.2);
      waveform2.begin(1, 647, masterWaveform);

      // random walk initial conditions
      for(int i=0; i<2; i++)
      {
        // velocities: initial conditions in -pi : +pi
        theta = PI*(random(2.0)-1.0);
        vx[i] = cos(theta);
        vy[i] = sin(theta);
        // positions: random in [0,L] x [0, L]
        x[i] = random(L);
        y[i] = random(L);
      }
    }

    void process(float k1, float k2) override {


        float knob_1 = k1;
        float knob_2 = k2;
        float dL;

        dL =  L + 100;

        v_var = v_0;

          // loop to "walk" randomly
        for(int i=0; i<2; i++)
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
         if(xn < 50)
            //xn += dL;
            xn += 10;
        else if (xn > dL)
          //xn -= dL;
          xn -= 10;

        if(yin < 0.01)
          yin += dL;
        else if (yin > dL)
          yin -= dL;
          x[i] = xn;
          y[i] = yin;
      }
      sine_fm1.frequency(knob_2*500 + 10);
      sine_fm2.frequency(knob_1*500 + 10);

      waveform1.frequency(x[0]);
      waveform2.frequency(x[1]);
    }

    AudioStream& getStream() override { return mixer1; }
    unsigned char getPort() override { return 0; }

  private:
      /* will be filled in */
    // GUItool: begin automatically generated code

    AudioMixer4              mixer1; //xy=961.75,589.75

    AudioSynthWaveform    waveform1;           //xy=588.5,279.5
    AudioSynthWaveform    waveform2;           //xy=589.5,228.5

    AudioSynthWaveformSineModulated sine_fm1;       //xy=840,305
    AudioSynthWaveformSineModulated sine_fm2;       //xy=845,388
    //AudioOutputI2S           i2s1;           //xy=1227.75,604.75
    AudioConnection          patchCord1;
    AudioConnection          patchCord2;
    AudioConnection          patchCord3;
    AudioConnection          patchCord4;

    int L, i, t;
    float theta, posx, posy, xn, yin;
    float v_0, v_var;//pw = pulse width
    float x[2], y[2], vx[2], vy[2]; // number depends on waveforms declared

};

REGISTER_PLUGIN(Rwalk_SineFM); // this is important, so that we can include the plugin in a bank
