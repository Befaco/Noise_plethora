// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"
#include "AudioStreamCustom.hpp"

class CombNoise : public Plugin {

public:

  CombNoise() {}

  ~CombNoise() override {}

  CombNoise(const CombNoise&) = delete;
  CombNoise& operator=(const CombNoise&) = delete;

  void init() override {
    for (int c = 0; c < 4; c++) {
      customStream.writePos[c] = 0;
      for (int i = 0; i < 2048; i++) {
        customStream.delayBuffer[c][i] = 0.0f;
      }
    }
  }

  void process(float k1, float k2) override {
    float delayInSamples = 44100.0f / (40.0f + pow(k1, 2) * 4960.0f);
    customStream.feedback = k2 * 0.95f;

    // 4 combs at delay ratios 1.0, 0.7, 0.5, 0.35
    customStream.delaySamples[0] = delayInSamples * 1.0f;
    customStream.delaySamples[1] = delayInSamples * 0.7f;
    customStream.delaySamples[2] = delayInSamples * 0.5f;
    customStream.delaySamples[3] = delayInSamples * 0.35f;

    // clamp delay lengths to valid buffer range
    for (int c = 0; c < 4; c++) {
      if (customStream.delaySamples[c] < 1.0f) customStream.delaySamples[c] = 1.0f;
      if (customStream.delaySamples[c] > 2047.0f) customStream.delaySamples[c] = 2047.0f;
    }
  }

  AudioStream& getStream() override { return customStream; }
  unsigned char getPort() override { return 0; }

private:

  class Stream : public AudioStreamCustom {
  public:
    float delayBuffer[4][2048] = {};
    int writePos[4] = {};
    float delaySamples[4] = {100.0f, 70.0f, 50.0f, 35.0f};
    float feedback = 0.0f;

    void fillBlock(audio_block_t *block) override {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        // Generate white noise input
        float input = random(-32768, 32768) / 32768.0f;
        float mixed = 0.0f;

        for (int c = 0; c < 4; c++) {
          int delay = (int)delaySamples[c];
          if (delay < 1) delay = 1;
          if (delay > 2047) delay = 2047;

          int readPos = writePos[c] - delay;
          if (readPos < 0) readPos += 2048;

          float out = input + feedback * delayBuffer[c][readPos];
          delayBuffer[c][writePos[c]] = out;

          writePos[c] = (writePos[c] + 1) & 2047; // mod 2048

          mixed += out;
        }

        // mix 4 combs equally (scale by 0.25)
        mixed *= 0.25f;

        // clamp and convert to int16
        if (mixed > 1.0f) mixed = 1.0f;
        if (mixed < -1.0f) mixed = -1.0f;
        block->data[i] = (int16_t)(mixed * 32767.0f);
      }
    }
  } customStream;
};

REGISTER_PLUGIN(CombNoise);
