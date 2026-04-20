// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"
#include "AudioStreamCustom.hpp"

class DualPulse : public Plugin {

public:

  DualPulse() {}

  ~DualPulse() override {}

  DualPulse(const DualPulse&) = delete;
  DualPulse& operator=(const DualPulse&) = delete;

  void init() override {
    customStream.value1 = 0.0f;
    customStream.value2 = 0.0f;
    customStream.counter1 = 0;
    customStream.counter2 = 0;
    customStream.samplesPerClock1 = 441;
    customStream.samplesPerClock2 = 441;
  }

  void process(float k1, float k2) override {
    float rate1 = 5.0f + pow(k1, 2) * 495.0f;
    float rate2 = 5.0f + pow(k2, 2) * 495.0f;
    int spc1 = (int)(44100.0f / rate1);
    if (spc1 < 1) spc1 = 1;
    int spc2 = (int)(44100.0f / rate2);
    if (spc2 < 1) spc2 = 1;
    customStream.samplesPerClock1 = spc1;
    customStream.samplesPerClock2 = spc2;
  }

  AudioStream& getStream() override { return customStream; }
  unsigned char getPort() override { return 0; }

private:

  class Stream : public AudioStreamCustom {
  public:
    float value1 = 0.0f;
    float value2 = 0.0f;
    int counter1 = 0;
    int counter2 = 0;
    int samplesPerClock1 = 441;
    int samplesPerClock2 = 441;

    void fillBlock(audio_block_t *block) override {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        counter1++;
        if (counter1 >= samplesPerClock1) {
          value1 = random(-32768, 32768) / 32768.0f;
          counter1 = 0;
        }

        counter2++;
        if (counter2 >= samplesPerClock2) {
          value2 = random(-32768, 32768) / 32768.0f;
          counter2 = 0;
        }

        float out = 0.5f * value1 + 0.5f * value2;

        // Add dither
        out += (random(0, 32768) / 32768.0f - 0.5f) * 0.01f;

        if (out > 1.0f) out = 1.0f;
        if (out < -1.0f) out = -1.0f;

        block->data[i] = (int16_t)(out * 32767.0f);
      }
    }
  } customStream;
};

REGISTER_PLUGIN(DualPulse);
