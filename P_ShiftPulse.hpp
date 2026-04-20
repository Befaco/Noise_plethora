// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"
#include "AudioStreamCustom.hpp"

class ShiftPulse : public Plugin {

public:

  ShiftPulse() {}

  ~ShiftPulse() override {}

  ShiftPulse(const ShiftPulse&) = delete;
  ShiftPulse& operator=(const ShiftPulse&) = delete;

  void init() override {
    for (int i = 0; i < 4; i++) {
      customStream.stages[i] = 0.0f;
      customStream.stageWeights[i] = 0.0f;
    }
    customStream.stageWeights[0] = 1.0f;
    customStream.sampleCounter = 0;
    customStream.samplesPerClock = 441;
  }

  void process(float k1, float k2) override {
    float rate = 5.0f + pow(k1, 2) * 295.0f;
    int spc = (int)(44100.0f / rate);
    if (spc < 1) spc = 1;
    customStream.samplesPerClock = spc;

    // Crossfade stage weights based on k2
    float w0 = 1.0f;
    float w1 = k2 * 3.0f;
    if (w1 > 1.0f) w1 = 1.0f;
    float w2 = k2 * 3.0f - 1.0f;
    if (w2 < 0.0f) w2 = 0.0f;
    if (w2 > 1.0f) w2 = 1.0f;
    float w3 = k2 * 3.0f - 2.0f;
    if (w3 < 0.0f) w3 = 0.0f;
    if (w3 > 1.0f) w3 = 1.0f;

    // Normalize weights
    float sum = w0 + w1 + w2 + w3;
    if (sum > 0.0f) {
      customStream.stageWeights[0] = w0 / sum;
      customStream.stageWeights[1] = w1 / sum;
      customStream.stageWeights[2] = w2 / sum;
      customStream.stageWeights[3] = w3 / sum;
    }
  }

  AudioStream& getStream() override { return customStream; }
  unsigned char getPort() override { return 0; }

private:

  class Stream : public AudioStreamCustom {
  public:
    float stages[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float stageWeights[4] = {1.0f, 0.0f, 0.0f, 0.0f};
    int sampleCounter = 0;
    int samplesPerClock = 441;

    void fillBlock(audio_block_t *block) override {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        sampleCounter++;
        if (sampleCounter >= samplesPerClock) {
          // Shift register: shift down
          stages[3] = stages[2];
          stages[2] = stages[1];
          stages[1] = stages[0];
          stages[0] = random(-32768, 32768) / 32768.0f;

          sampleCounter = 0;
        }

        float out = 0.0f;
        for (int s = 0; s < 4; s++) {
          out += stageWeights[s] * stages[s];
        }

        // NaN/Inf guard
        if (isnan(out) || isinf(out)) {
          out = 0.0f;
        }

        out += (random(0, 32768) / 32768.0f - 0.5f) * 0.01f;

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

REGISTER_PLUGIN(ShiftPulse);
