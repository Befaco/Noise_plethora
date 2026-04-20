// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"
#include "AudioStreamCustom.hpp"

class SubHarmonic : public Plugin {

public:

  SubHarmonic() {}

  ~SubHarmonic() override {}

  SubHarmonic(const SubHarmonic&) = delete;
  SubHarmonic& operator=(const SubHarmonic&) = delete;

  void init() override {
    customStream.srcFreq = 440.0f;
    customStream.srcPhase = 0.0f;

    for (int i = 0; i < 4; i++) {
      customStream.divCounters[i] = 0;
      customStream.divStates[i] = false;
      customStream.divWeights[i] = 0.0f;
    }
    customStream.divWeights[0] = 1.0f;
    customStream.prevSamplePositive = true;
  }

  void process(float k1, float k2) override {
    float freq = 100.0f + pow(k1, 2.0f) * 4000.0f;
    customStream.srcFreq = freq;

    // Crossfade subharmonic levels based on k2
    customStream.divWeights[0] = 1.0f; // div2 always active
    float w1 = k2 * 3.0f;
    customStream.divWeights[1] = w1 < 0.0f ? 0.0f : (w1 > 1.0f ? 1.0f : w1);
    float w2 = k2 * 3.0f - 1.0f;
    customStream.divWeights[2] = w2 < 0.0f ? 0.0f : (w2 > 1.0f ? 1.0f : w2);
    float w3 = k2 * 3.0f - 2.0f;
    customStream.divWeights[3] = w3 < 0.0f ? 0.0f : (w3 > 1.0f ? 1.0f : w3);
  }

  AudioStream& getStream() override { return customStream; }
  unsigned char getPort() override { return 0; }

private:

  class Stream : public AudioStreamCustom {
  public:
    float srcFreq = 440.0f;
    float srcPhase = 0.0f;

    int divRatios[4] = {2, 3, 5, 7};
    int divCounters[4] = {0, 0, 0, 0};
    bool divStates[4] = {false, false, false, false};
    float divWeights[4] = {1.0f, 0.0f, 0.0f, 0.0f};

    bool prevSamplePositive = true;

    void fillBlock(audio_block_t *block) override {
      float phaseInc = srcFreq / 44100.0f;

      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        // Square wave oscillator via phase accumulator
        srcPhase += phaseInc;
        if (srcPhase >= 1.0f) srcPhase -= 1.0f;

        float srcFloat = (srcPhase < 0.5f) ? 1.0f : -1.0f;
        bool currentPositive = (srcFloat >= 0.0f);

        // Detect zero crossing (sign change)
        if (currentPositive != prevSamplePositive) {
          for (int d = 0; d < 4; d++) {
            divCounters[d]++;
            if (divCounters[d] >= divRatios[d]) {
              divStates[d] = !divStates[d];
              divCounters[d] = 0;
            }
          }
        }
        prevSamplePositive = currentPositive;

        // Mix: source + subharmonics, normalized to prevent clipping
        float totalWeight = 0.15f;
        for (int d = 0; d < 4; d++) totalWeight += divWeights[d] * 0.35f;
        float norm = 0.9f / totalWeight;

        float mix = 0.15f * norm * srcFloat;
        for (int d = 0; d < 4; d++) {
          float divValue = divStates[d] ? 0.35f : -0.35f;
          mix += divWeights[d] * norm * divValue;
        }

        int32_t out = (int32_t)(mix * 32767.0f);
        if (out > 32767) out = 32767;
        if (out < -32767) out = -32767;
        block->data[i] = (int16_t)out;
      }
    }
  } customStream;
};

REGISTER_PLUGIN(SubHarmonic);
