// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"
#include "AudioStreamCustom.hpp"

class NoiseBells : public Plugin {

public:

  NoiseBells() {}

  ~NoiseBells() override {}

  NoiseBells(const NoiseBells&) = delete;
  NoiseBells& operator=(const NoiseBells&) = delete;

  void init() override {
    for (int i = 0; i < NUM_RESONATORS; i++) {
      customStream.y1[i] = 0.0f;
      customStream.y2[i] = 0.0f;
      customStream.coeff_a[i] = 0.0f;
      customStream.coeff_b[i] = 0.0f;
      customStream.coeff_g[i] = 0.0f;
    }

    // initial resonator setup
    updateResonators(500.0f, 0.5f);
  }

  void process(float k1, float k2) override {
    float freq = 80.0f + pow(k1, 2) * 4000.0f;
    updateResonators(freq, k2);
  }

  AudioStream& getStream() override { return customStream; }
  unsigned char getPort() override { return 0; }

private:

  static const int NUM_RESONATORS = 4;

  void updateResonators(float fundamental, float inharmonicity) {
    // bell-like inharmonic ratios
    float stretch = 1.0f + inharmonicity * 2.0f;
    float freqs[NUM_RESONATORS];
    freqs[0] = fundamental;
    freqs[1] = fundamental * (1.0f + 0.59f * stretch);
    freqs[2] = fundamental * (1.0f + 1.33f * stretch);
    freqs[3] = fundamental * (1.0f + 2.14f * stretch);

    float baseR = 0.9992f;

    for (int i = 0; i < NUM_RESONATORS; i++) {
      float f = freqs[i];
      if (f > 20000.0f) f = 20000.0f;
      if (f < 20.0f) f = 20.0f;

      float r = baseR - i * 0.0003f;
      float w = 2.0f * M_PI * f / 44100.0f;

      customStream.coeff_a[i] = 2.0f * r * cos(w);
      customStream.coeff_b[i] = r * r;
      customStream.coeff_g[i] = 1.0f - r;
    }

    // amplitude rolloff for higher partials
    customStream.gains[0] = 1.0f;
    customStream.gains[1] = 0.7f;
    customStream.gains[2] = 0.4f;
    customStream.gains[3] = 0.25f;
  }

  class Stream : public AudioStreamCustom {
  public:
    static const int NUM_RESONATORS = 4;

    float y1[NUM_RESONATORS] = {};
    float y2[NUM_RESONATORS] = {};
    float coeff_a[NUM_RESONATORS] = {};
    float coeff_b[NUM_RESONATORS] = {};
    float coeff_g[NUM_RESONATORS] = {};
    float gains[NUM_RESONATORS] = {};

    void fillBlock(audio_block_t *block) override {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        float input = random(-32768, 32768) / 32768.0f;

        float mixed = 0.0f;

        for (int r = 0; r < NUM_RESONATORS; r++) {
          // 2-pole resonator
          float out = coeff_a[r] * y1[r] - coeff_b[r] * y2[r] + coeff_g[r] * input;

          y2[r] = y1[r];
          y1[r] = out;

          // safety: reset on NaN/inf
          if (isnan(out) || isinf(out)) {
            y1[r] = 0.0f;
            y2[r] = 0.0f;
            out = 0.0f;
          }

          mixed += out * gains[r];
        }

        // scale down (resonators can be loud)
        mixed *= 0.15f;

        if (mixed > 1.0f) mixed = 1.0f;
        if (mixed < -1.0f) mixed = -1.0f;
        block->data[i] = (int16_t)(mixed * 32767.0f);
      }
    }
  } customStream;
};

REGISTER_PLUGIN(NoiseBells);
