// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"
#include "AudioStreamCustom.hpp"

class LogisticNoise : public Plugin {

public:

  LogisticNoise() {}

  ~LogisticNoise() override {}

  LogisticNoise(const LogisticNoise&) = delete;
  LogisticNoise& operator=(const LogisticNoise&) = delete;

  void init() override {
    customStream.x = 0.5f;
    customStream.holdCounter = 0;
    customStream.samplesPerIteration = 1;
    customStream.r = 3.8f;
  }

  void process(float k1, float k2) override {
    customStream.r = 3.5f + k1 * 0.5f;
    int spi = 1 + (int)((1.0f - k2) * 30);
    if (spi < 1) spi = 1;
    customStream.samplesPerIteration = spi;
  }

  AudioStream& getStream() override { return customStream; }
  unsigned char getPort() override { return 0; }

private:

  class Stream : public AudioStreamCustom {
  public:
    float x = 0.5f;
    float r = 3.8f;
    int holdCounter = 0;
    int samplesPerIteration = 1;

    void fillBlock(audio_block_t *block) override {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        holdCounter++;
        if (holdCounter >= samplesPerIteration) {
          x = r * x * (1.0f - x);
          holdCounter = 0;

          // Reset on numerical instability
          if (x < 0.0f || x > 1.0f || isnan(x) || isinf(x)) {
            x = 0.5f + (random(0, 32768) / 32768.0f - 0.5f) * 0.1f;
          }
        }

        float out = x * 2.0f - 1.0f;
        int32_t sample = (int32_t)(out * 32767.0f);
        if (sample > 32767) sample = 32767;
        if (sample < -32767) sample = -32767;
        block->data[i] = (int16_t)sample;
      }
    }
  } customStream;
};

REGISTER_PLUGIN(LogisticNoise);
