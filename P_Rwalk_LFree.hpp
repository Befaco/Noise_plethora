// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Julia Mugica / Ivan Paz
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"

class Rwalk_LFree : public Plugin {

  public:

    Rwalk_LFree()
    : patchCord1(pwm2, 0, mixer5, 1)
    , patchCord2(pwm1, 0, mixer5, 0)
    , patchCord3(pwm3, 0, mixer5, 2)
    , patchCord4(pwm4, 0, mixer5, 3)
    , patchCord5(mixer5, freeverb1)
    { }

    ~Rwalk_LFree() override {}

    // delete copy constructors
    Rwalk_LFree(const Rwalk_LFree&) = delete;
    Rwalk_LFree& operator=(const Rwalk_LFree&) = delete;

    void init() override {

      L = 500; // Size of box: maximum frequency
      v_0 = 5; // speed: size of step in frequency units.

      mixer5.gain(0,1);
      mixer5.gain(1,1);
      mixer5.gain(2,1);
      mixer5.gain(3,1);


      pwm1.frequency(794);
      pwm1.amplitude(1);

      pwm2.frequency(647);
      pwm2.amplitude(1);

      pwm3.frequency(524);
      pwm3.amplitude(1);

      pwm4.frequency(444);
      pwm4.amplitude(1);

      // random walk initial conditions
      for(int i=0; i<4; i++)
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
        float dL, fv;

        dL =  knob_1*L + 50;

        v_var = v_0;

        fv = knob_2;
        freeverb1.roomsize(fv);

          // loop to "walk" randomly
        for(int i=0; i<4; i++)
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
         if(xn < 40)
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
      pwm1.frequency(x[0]);
      pwm2.frequency(x[1]);
      pwm3.frequency(x[2]);
      pwm4.frequency(x[3]);
    }

    AudioStream& getStream() override { return freeverb1; }
    unsigned char getPort() override { return 0; }

  private:
      /* will be filled in */
    // GUItool: begin automatically generated code

    AudioMixer4              mixer5; //xy=961.75,589.75

    AudioSynthWaveformPWM    pwm2;           //xy=588.5,279.5
    AudioSynthWaveformPWM    pwm1;           //xy=589.5,228.5
    AudioSynthWaveformPWM    pwm3;           //xy=593.5,331.5
    AudioSynthWaveformPWM    pwm4;           //xy=596.5,375.5

    AudioEffectFreeverb      freeverb1;

    //AudioOutputI2S           i2s1;           //xy=1227.75,604.75
    AudioConnection          patchCord1;
    AudioConnection          patchCord2;
    AudioConnection          patchCord3;
    AudioConnection          patchCord4;
    AudioConnection          patchCord5;

    int L, i, t;
    float theta, posx, posy, xn, yin;
    float v_0, v_var;//pw = pulse width
    float x[4], y[4], vx[4], vy[4]; // number depends on waveforms declared

};

REGISTER_PLUGIN(Rwalk_LFree); // this is important, so that we can include the plugin in a bank
