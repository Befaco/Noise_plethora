#### CONVERSION GUIDE

#### This guide is about how to convert a program made for Teensy (using the Teensy Audio Library), to the Noise Plethora "plugin" format.
We assume you have some previous programming knowledge and you are familiar with the Arduino IDE and C++ 

For info about how to create your programs please check:

https://www.youtube.com/watch?v=wqt55OAabVs

https://hackaday.io/project/8292-microcontroller-audio-workshop-had-supercon-2015


So here's a simple plugin as exported by the Teensy audio tool:

![simple patch](https://user-images.githubusercontent.com/62393344/141118046-9cf9c5f6-6a94-4898-955a-787e93e1b85f.png)

    #include <Audio.h>
    #include <Wire.h>
    #include <SPI.h>
    #include <SD.h>
    #include <SerialFlash.h>
    #define CV_ADC_MIDPOINT 511

    // GUItool: begin automatically generated code
    AudioSynthWaveformModulated waveformMod1;
    AudioEffectFreeverb      freeverb1;
    AudioMixer4              mixer1;
    AudioOutputI2S           audioOut;
    AudioConnection          patchCord1(waveformMod1, 0, mixer1, 0);
    AudioConnection          patchCord2(waveformMod1, freeverb1);
    AudioConnection          patchCord3(freeverb1, 0, mixer1, 1);
    AudioConnection          patchCord4(mixer1, 0, audioOut, 0);
    AudioControlSGTL5000     SGTL5000;
    // GUItool: end automatically generated code

    void setup() {
     AudioMemory(25);
     SGTL5000.enable();
     SGTL5000.lineOutLevel(13);

     waveformMod1.begin(1, 200, WAVEFORM_SAMPLE_HOLD);
     waveformMod1.frequencyModulation(10);

     freeverb1.damping(1);
     freeverb1.roomsize(0.5);

     mixer1.gain(0, 1);
     mixer1.gain(1, 0);      
    }

    void loop() {
      // Read CV and knobs,sum them and scale to 0-1.0
      float knob_1 = min(1023, analogRead(A8) + (CV_ADC_MIDPOINT - analogRead(A10))) /1023.0;
      float knob_2 = min(1023, analogRead(A2) + (CV_ADC_MIDPOINT - analogRead(A11))) /1023.0;

      waveformMod1.frequency(5000*knob_1+15);

      mixer1.gain(0, 1-knob_2);
      mixer1.gain(1, knob_2*4);
    }


Note, on this example, "knob_1" is the analog read of the pot/CV X and knob_2 is the analog read of the pot/CV Y of the Noise Plethora.
On your program, you should follow the same structure.

Before converting the program, you can try to upload it "AS IS" to the module and check everything works without conversion.

To prepare it for use with the Noise Plethora, you need to convert it to a C++ class inheriting from the `Plugin` base class. In general, you can put each plugin into a header file. By convention, Noise Plethora uses the `.hpp` extension for C++ header files.

* The Noise Plethora running code is handling setup() and loop() for the audio interface, and already includes all of the necessary system headers. So you can delete the entire first portion of the file (up to the first empty line). Replace with:

          #pragma once
          #include "Plugin.hpp"

* Next, let's make the class:
    ```
      class S_H : public Plugin {
        public:
      S_H() { /* will be filled in Public class section */ }
          ~S_H() override {}

          // delete copy constructors
          S_H(const S_H&) = delete;
          S_H& operator=(const S_H&) = delete;
          private:
          /* will be filled in Private class section */
      };
        REGISTER_PLUGIN(S_H); // Do not forget to register the plugin (ex. S_H), so that we can include the plugin in a bank.

    ```

**Private class section**
* The global variables in the next section need to become private class members, EXCEPT for the `AudioOutputI2S audioOut` and `AudioControlSGTL5000 SGTL5000`, as well as the patchcord connecting the signal to the AudioOutputI2S (in this example, `patchCord4(mixer1, 0, audioOut, 0);`). Also, the initialization of the AudioConnections will need to be moved to the public section. So the private section of your class should look like this:
```
              private:
                AudioSynthWaveformModulated waveformMod1;
                AudioEffectFreeverb freeverb1;
                AudioMixer4 mixer1;
                AudioConnection patchCord1;
                AudioConnection patchCord2;
                AudioConnection patchCord3;
```

 **Public class section**
* Now we can fill out the class constructor with the initialization of AudioConnections patchcords from the original source of the teensy audio tool:

```
              public:
                S_H()
                : patchCord1(waveformMod1, 0, mixer1, 0)
                , patchCord2(waveformMod1, freeverb1)
                , patchCord3(freeverb1, 0, mixer1, 1)
                { }              
```
* The class requires some additional member functions to be overridden from the base class (Plugin), in order for it to work properly:
```
              void init() override; // set up the plugin, from setup()
              void process(float k1, float k2) override; // process audio, from loop()
              AudioStream& getStream() override; // communicate the output stream to the plugin loader
              unsigned char getPort() override; // communicate the output port to the plugin loader
```
In our case, this will look like:
```
              void init() override {
                waveformMod1.begin(1, 200, WAVEFORM_SAMPLE_HOLD);
                waveformMod1.frequencyModulation(10);

                freeverb1.damping(1);
                freeverb1.roomsize(0.5);

                mixer1.gain(0, 1);
                mixer1.gain(1, 0);
              }

              // process() is called with 2 float args between 0.-1.,
              // representing the value of the two hardware potentiometers
              void process(float k1, float k2) override {
                waveformMod1.frequency(5000 * k1 + 15);
                mixer1.gain(0, 1 - k2);
                mixer1.gain(1, k2 * 4);
              }

              // mixer1 was connected to the audio output in the original source (patchCord4(mixer1, 0, audioOut, 0);), return it
              AudioStream& getStream() override { return mixer1; }
              // port 0 of mixer1 was connected to the audio output in the original source, return 0.
              unsigned char getPort() override { return 0; }
```
* and that's it for the class definition.


**Add plugin to a bank**
* To add this plugin to a bank, open the `Banks_Def.hpp` file and add the name of the class to one of the bank definitions following the model of the file. Each bank may have a maximum of 10 entries.
Also, add to `Banks.cpp` your plugin  (ex. #include "P_S_H.hpp").


* The complete source is below:

`S_H.hpp`:
```
        #pragma once

        #include "Plugin.hpp"

        class S_H : public Plugin {
        public:
          S_H()
            : patchCord1(waveformMod1, 0, mixer1, 0)
            , patchCord2(waveformMod1, freeverb1)
            , patchCord3(freeverb1, 0, mixer1, 1)
            { }

          ~S_H() override {}

          S_H(const S_H&) = delete;
          S_H& operator=(const S_H&) = delete;

          void init() override {
            waveformMod1.begin(1, 200, WAVEFORM_SAMPLE_HOLD);
            waveformMod1.frequencyModulation(10);

            freeverb1.damping(1);
            freeverb1.roomsize(0.5);

            mixer1.gain(0, 1);
            mixer1.gain(1, 0);
          }

          void process(float k1, float k2) override {
            waveformMod1.frequency(5000 * k1 + 15);
            mixer1.gain(0, 1 - k2);
            mixer1.gain(1, k2 * 4);
          }

          AudioStream& getStream() override { return mixer1; }
          unsigned char getPort() override { return 0; }

        private:
          AudioSynthWaveformModulated waveformMod1;
          AudioEffectFreeverb      freeverb1;
          AudioMixer4              mixer1;
          AudioConnection          patchCord1;
          AudioConnection          patchCord2;
          AudioConnection          patchCord3;
        };

        REGISTER_PLUGIN(S_H);
```
