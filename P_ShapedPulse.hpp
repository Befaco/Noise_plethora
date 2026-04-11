// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"
#include "AudioStreamCustom.hpp"

class ShapedPulse : public Plugin {

public:

  ShapedPulse() {}

  ~ShapedPulse() override {}

  ShapedPulse(const ShapedPulse&) = delete;
  ShapedPulse& operator=(const ShapedPulse&) = delete;

  void init() override {
    customStream.currentValue = 0.0f;
    customStream.sampleCounter = 0;
    customStream.samplesPerClock = 441;
    customStream.shape = 0.0f;
  }

  void process(float k1, float k2) override {
    float rate = 5.0f + pow(k1, 2) * 495.0f;
    int spc = (int)(44100.0f / rate);
    if (spc < 1) spc = 1;
    customStream.samplesPerClock = spc;

    customStream.shape = k2;
  }

  AudioStream& getStream() override { return customStream; }
  unsigned char getPort() override { return 0; }

private:

  class Stream : public AudioStreamCustom {
  public:
    float currentValue = 0.0f;
    int sampleCounter = 0;
    int samplesPerClock = 441;
    float shape = 0.0f;

    void fillBlock(audio_block_t *block) override {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        sampleCounter++;
        if (sampleCounter >= samplesPerClock) {
          float val;

          if (shape < 0.5f) {
            // Blend uniform and triangular
            float blend = shape * 2.0f;
            float uniform = random(-32768, 32768) / 32768.0f;
            float tri = (random(0, 32768) / 32768.0f + random(0, 32768) / 32768.0f) - 1.0f;
            val = uniform * (1.0f - blend) + tri * blend;
          }
          else {
            // Blend triangular and peaked (pseudo-gaussian)
            float blend = (shape - 0.5f) * 2.0f;
            float tri = (random(0, 32768) / 32768.0f + random(0, 32768) / 32768.0f) - 1.0f;
            float peaked = (random(0, 32768) / 32768.0f + random(0, 32768) / 32768.0f
                          + random(0, 32768) / 32768.0f + random(0, 32768) / 32768.0f) * 0.5f - 1.0f;
            val = tri * (1.0f - blend) + peaked * blend;
          }

          if (val > 1.0f) val = 1.0f;
          if (val < -1.0f) val = -1.0f;

          currentValue = val;
          sampleCounter = 0;
        }

        // NaN/Inf guard
        if (isnan(currentValue) || isinf(currentValue)) {
          currentValue = 0.0f;
        }

        float out = currentValue + (random(0, 32768) / 32768.0f - 0.5f) * 0.01f;

        if (out > 1.0f) out = 1.0f;
        if (out < -1.0f) out = -1.0f;

        int32_t sample = (int32_t)(out * 32767.0f);
        if (sample > 32767) sample = 32767;
        if (sample < -32767) sample = -32767;
        block->data[i] = (int16_t)sample;
      }
    }
  } customStream;
};

REGISTER_PLUGIN(ShapedPulse);
