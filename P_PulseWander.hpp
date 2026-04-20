// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"
#include "AudioStreamCustom.hpp"

class PulseWander : public Plugin {

public:

  PulseWander() {}

  ~PulseWander() override {}

  PulseWander(const PulseWander&) = delete;
  PulseWander& operator=(const PulseWander&) = delete;

  void init() override {
    customStream.currentValue = random(-32768, 32768) / 32768.0f;
    customStream.targetValue = random(-32768, 32768) / 32768.0f;
    customStream.smoothAlpha = 0.1f;
    customStream.sampleCounter = 0;
    customStream.samplesPerTarget = 441;
  }

  void process(float k1, float k2) override {
    float rate = 5.0f + pow(k1, 2) * 395.0f;
    int spt = (int)(44100.0f / rate);
    if (spt < 1) spt = 1;
    customStream.samplesPerTarget = spt;

    float minAlpha = 3.0f / (float)customStream.samplesPerTarget;
    customStream.smoothAlpha = minAlpha + (1.0f - k2) * (1.0f - minAlpha);
  }

  AudioStream& getStream() override { return customStream; }
  unsigned char getPort() override { return 0; }

private:

  class Stream : public AudioStreamCustom {
  public:
    float currentValue = 0.0f;
    float targetValue = 0.0f;
    float smoothAlpha = 0.1f;
    int sampleCounter = 0;
    int samplesPerTarget = 441;

    void fillBlock(audio_block_t *block) override {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        sampleCounter++;
        if (sampleCounter >= samplesPerTarget) {
          targetValue = random(-32768, 32768) / 32768.0f;
          sampleCounter = 0;
        }

        currentValue = currentValue * (1.0f - smoothAlpha) + targetValue * smoothAlpha;

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

REGISTER_PLUGIN(PulseWander);
