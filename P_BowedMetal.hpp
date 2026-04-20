// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"
#include "AudioStreamCustom.hpp"

class BowedMetal : public Plugin {

public:

  BowedMetal() {}

  ~BowedMetal() override {}

  BowedMetal(const BowedMetal&) = delete;
  BowedMetal& operator=(const BowedMetal&) = delete;

  void init() override {
    customStream.noiseLevel = 0.5f;

    for (int i = 0; i < NUM_RES; i++) {
      customStream.y1[i] = 0.0f;
      customStream.y2[i] = 0.0f;
    }

    updateResonators(200.0f, 0.5f, 0.5f);
  }

  void process(float k1, float k2) override {
    float freq = 30.0f + pow(k1, 2) * 2000.0f;

    // k2 low = struck (short ring, louder noise burst)
    // k2 high = bowed (long ring, quieter continuous noise)
    float ringTime = 0.9985f + k2 * 0.0013f;
    float noiseLevel = 0.4f - k2 * 0.32f;
    customStream.noiseLevel = noiseLevel;

    updateResonators(freq, k2, ringTime);
  }

  AudioStream& getStream() override { return customStream; }
  unsigned char getPort() override { return 0; }

private:

  static const int NUM_RES = 8;

  // Metal plate / cymbal mode ratios
  static constexpr float modeRatios[NUM_RES] = {
    1.000f, 1.183f, 1.506f, 1.741f,
    2.098f, 2.534f, 2.917f, 3.483f
  };

  void updateResonators(float fundamental, float k2, float ringTime) {
    for (int i = 0; i < NUM_RES; i++) {
      float f = fundamental * modeRatios[i];
      if (f > 19000.0f) f = 19000.0f;
      if (f < 20.0f) f = 20.0f;

      // higher modes decay faster
      float r = ringTime - i * 0.0002f;
      if (r < 0.99f) r = 0.99f;

      float w = 2.0f * M_PI * f / 44100.0f;

      customStream.coeff_a[i] = 2.0f * r * cos(w);
      customStream.coeff_b[i] = r * r;
      customStream.coeff_g[i] = 1.0f - r;
    }

    // dense amplitude distribution
    customStream.gains[0] = 1.0f;
    customStream.gains[1] = 0.9f;
    customStream.gains[2] = 0.85f;
    customStream.gains[3] = 0.75f;
    customStream.gains[4] = 0.65f;
    customStream.gains[5] = 0.55f;
    customStream.gains[6] = 0.45f;
    customStream.gains[7] = 0.35f;
  }

  class Stream : public AudioStreamCustom {
  public:
    static const int NUM_RES = 8;

    float y1[NUM_RES] = {};
    float y2[NUM_RES] = {};
    float coeff_a[NUM_RES] = {};
    float coeff_b[NUM_RES] = {};
    float coeff_g[NUM_RES] = {};
    float gains[NUM_RES] = {};
    float noiseLevel = 0.5f;

    void fillBlock(audio_block_t *block) override {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        float input = (random(-32768, 32768) / 32768.0f) * noiseLevel;

        float mixed = 0.0f;

        for (int r = 0; r < NUM_RES; r++) {
          float out = coeff_a[r] * y1[r] - coeff_b[r] * y2[r] + coeff_g[r] * input;

          y2[r] = y1[r];
          y1[r] = out;

          if (isnan(out) || isinf(out)) {
            y1[r] = 0.0f;
            y2[r] = 0.0f;
            out = 0.0f;
          }

          mixed += out * gains[r];
        }

        mixed *= 0.1f;

        if (mixed > 1.0f) mixed = 1.0f;
        if (mixed < -1.0f) mixed = -1.0f;
        block->data[i] = (int16_t)(mixed * 32767.0f);
      }
    }
  } customStream;
};

REGISTER_PLUGIN(BowedMetal);
