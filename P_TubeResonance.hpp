// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"
#include "AudioStreamCustom.hpp"

class TubeResonance : public Plugin {

public:

  TubeResonance() {}

  ~TubeResonance() override {}

  TubeResonance(const TubeResonance&) = delete;
  TubeResonance& operator=(const TubeResonance&) = delete;

  void init() override {
    for (int c = 0; c < 4; c++) {
      customStream.writePos[c] = 0;
      for (int i = 0; i < 2048; i++) {
        customStream.buf[c][i] = 0.0f;
      }
    }
    customStream.fundamentalFreq = 200.0f;
    customStream.feedback = 0.85f;
    customStream.excitation = 0.5f;
  }

  void process(float k1, float k2) override {
    customStream.fundamentalFreq = 40.0f + pow(k1, 2) * 960.0f;
    customStream.excitation = k2;
    customStream.feedback = 0.7f + (1.0f - k2) * 0.25f;

    // compute delay lengths for 4 harmonics
    for (int c = 0; c < 4; c++) {
      float harmonic = (float)(c + 1);
      float freq = customStream.fundamentalFreq * harmonic;
      float delaySamples = 44100.0f / freq;
      customStream.delayLen[c] = (int)delaySamples;
      if (customStream.delayLen[c] < 1) customStream.delayLen[c] = 1;
      if (customStream.delayLen[c] > 2047) customStream.delayLen[c] = 2047;

      // feedback per harmonic scaled by 1/harmonic_number
      customStream.harmonicFeedback[c] = customStream.feedback / harmonic;
    }
  }

  AudioStream& getStream() override { return customStream; }
  unsigned char getPort() override { return 0; }

private:

  class Stream : public AudioStreamCustom {
  public:
    float buf[4][2048] = {};
    int writePos[4] = {};
    int delayLen[4] = {100, 50, 33, 25};
    float harmonicFeedback[4] = {};
    float fundamentalFreq = 200.0f;
    float feedback = 0.85f;
    float excitation = 0.5f;

    void fillBlock(audio_block_t *block) override {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        // Generate white noise input scaled by excitation
        float input = (random(-32768, 32768) / 32768.0f) * excitation;
        float mixed = 0.0f;

        // process 4 comb filters in parallel
        for (int c = 0; c < 4; c++) {
          int readPos = writePos[c] - delayLen[c];
          if (readPos < 0) readPos += 2048;

          float delayOut = buf[c][readPos];
          float out = input + harmonicFeedback[c] * delayOut;
          buf[c][writePos[c]] = out;

          writePos[c] = (writePos[c] + 1) & 2047; // mod 2048

          mixed += out;
        }

        // mix 4 combs equally
        mixed *= 0.25f;

        // clamp and convert to int16
        if (mixed > 1.0f) mixed = 1.0f;
        if (mixed < -1.0f) mixed = -1.0f;
        block->data[i] = (int16_t)(mixed * 32767.0f);
      }
    }
  } customStream;
};

REGISTER_PLUGIN(TubeResonance);
