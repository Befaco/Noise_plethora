// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"
#include "AudioStreamCustom.hpp"

class FeedbackFM : public Plugin {

public:

  FeedbackFM() {}

  ~FeedbackFM() override {}

  FeedbackFM(const FeedbackFM&) = delete;
  FeedbackFM& operator=(const FeedbackFM&) = delete;

  void init() override {
    customStream.phase = 0.0f;
    customStream.prevOut = 0.0f;
    customStream.currentFreq = 440.0f;
    customStream.currentFeedback = 0.0f;
  }

  void process(float k1, float k2) override {
    customStream.currentFreq = 20.0f + pow(k1, 2.0f) * 5000.0f;
    customStream.currentFeedback = k2 * 2.5f;
  }

  AudioStream& getStream() override { return customStream; }
  unsigned char getPort() override { return 0; }

private:

  class Stream : public AudioStreamCustom {
  public:
    float phase = 0.0f;
    float prevOut = 0.0f;
    float currentFreq = 440.0f;
    float currentFeedback = 0.0f;

    void fillBlock(audio_block_t *block) override {
      const float twoPi = 2.0f * 3.14159265358979323846f;
      const float phaseInc = twoPi * currentFreq / 44100.0f;

      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        phase += phaseInc;
        if (phase >= twoPi) phase -= twoPi;

        float out = sin(phase + currentFeedback * prevOut);
        prevOut = out;

        int32_t sample = (int32_t)(out * 32767.0f);
        if (sample > 32767) sample = 32767;
        if (sample < -32767) sample = -32767;
        block->data[i] = (int16_t)sample;
      }
    }
  } customStream;
};

REGISTER_PLUGIN(FeedbackFM);
