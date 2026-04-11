// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"
#include "AudioStreamCustom.hpp"

class NoiseSlew : public Plugin {

public:

  NoiseSlew() {}

  ~NoiseSlew() override {}

  NoiseSlew(const NoiseSlew&) = delete;
  NoiseSlew& operator=(const NoiseSlew&) = delete;

  void init() override {
    customStream.slewState = 0.0f;
    customStream.alpha = 0.01f;
    customStream.mix = 0.0f;
  }

  void process(float k1, float k2) override {
    customStream.alpha = 0.0003f + pow(k1, 3) * 0.15f;
    customStream.mix = k2;
  }

  AudioStream& getStream() override { return customStream; }
  unsigned char getPort() override { return 0; }

private:

  class Stream : public AudioStreamCustom {
  public:
    float slewState = 0.0f;
    float alpha = 0.01f;
    float mix = 0.0f;

    void fillBlock(audio_block_t *block) override {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        // Generate white noise sample
        float noiseSample = random(-32768, 32768) / 32768.0f;

        slewState = slewState * (1.0f - alpha) + noiseSample * alpha;

        // NaN guard
        if (isnan(slewState) || isinf(slewState)) {
          slewState = 0.0f;
        }

        float out = slewState * (1.0f - mix * 0.7f) + noiseSample * mix * 0.3f;

        if (out > 1.0f) out = 1.0f;
        if (out < -1.0f) out = -1.0f;

        block->data[i] = (int16_t)(out * 32767.0f);
      }
    }
  } customStream;
};

REGISTER_PLUGIN(NoiseSlew);
