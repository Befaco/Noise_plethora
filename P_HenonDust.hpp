// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"
#include "AudioStreamCustom.hpp"

class HenonDust : public Plugin {

public:

  HenonDust() {}

  ~HenonDust() override {}

  HenonDust(const HenonDust&) = delete;
  HenonDust& operator=(const HenonDust&) = delete;

  void init() override {
    customStream.hx = 0.1f;
    customStream.hy = 0.1f;
    customStream.a = 1.4f;
    customStream.b = 0.3f;
  }

  void process(float k1, float k2) override {
    // focus k1 on the interesting chaos transition zone (1.15-1.4)
    customStream.a = 1.15f + k1 * 0.25f;
    customStream.b = 0.15f + k2 * 0.2f;
  }

  AudioStream& getStream() override { return customStream; }
  unsigned char getPort() override { return 0; }

private:

  class Stream : public AudioStreamCustom {
  public:
    float hx = 0.1f;
    float hy = 0.1f;
    float a = 1.4f;
    float b = 0.3f;

    void fillBlock(audio_block_t *block) override {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        float new_x = 1.0f - a * hx * hx + hy;
        float new_y = b * hx;
        hx = new_x;
        hy = new_y;

        // Reset on divergence or NaN
        if (isnan(hx) || isinf(hx) || isnan(hy) || isinf(hy)) {
          hx = random(-32768, 32768) / 32768.0f * 0.5f;
          hy = random(-32768, 32768) / 32768.0f * 0.5f;
        }

        // Normalize hx from [-1.5, 1.5] to [-1, 1]
        float out = hx;
        if (out > 1.5f) out = 1.5f;
        if (out < -1.5f) out = -1.5f;
        out /= 1.5f;

        int32_t sample = (int32_t)(out * 32767.0f);
        if (sample > 32767) sample = 32767;
        if (sample < -32767) sample = -32767;
        block->data[i] = (int16_t)sample;
      }
    }
  } customStream;
};

REGISTER_PLUGIN(HenonDust);
