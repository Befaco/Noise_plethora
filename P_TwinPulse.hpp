// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"
#include "AudioStreamCustom.hpp"

class TwinPulse : public Plugin {

public:

  TwinPulse() {}

  ~TwinPulse() override {}

  TwinPulse(const TwinPulse&) = delete;
  TwinPulse& operator=(const TwinPulse&) = delete;

  void init() override {
    customStream.currentA = random(-32768, 32768) / 32768.0f;
    customStream.targetA = random(-32768, 32768) / 32768.0f;
    customStream.currentB = random(-32768, 32768) / 32768.0f;
    customStream.targetB = random(-32768, 32768) / 32768.0f;
    customStream.counterA = 0;
    customStream.counterB = 0;
    customStream.alpha = 0.1f;
    customStream.correlation = 0.0f;
    customStream.samplesPerTarget = 441;
  }

  void process(float k1, float k2) override {
    float rate = 5.0f + pow(k1, 2) * 395.0f;
    int spt = (int)(44100.0f / rate);
    if (spt < 1) spt = 1;
    customStream.samplesPerTarget = spt;

    float a = 4.0f / (float)customStream.samplesPerTarget;
    if (a > 0.5f) a = 0.5f;
    customStream.alpha = a;

    customStream.correlation = k2;
  }

  AudioStream& getStream() override { return customStream; }
  unsigned char getPort() override { return 0; }

private:

  class Stream : public AudioStreamCustom {
  public:
    float currentA = 0.0f;
    float targetA = 0.0f;
    float currentB = 0.0f;
    float targetB = 0.0f;
    int counterA = 0;
    int counterB = 0;
    int samplesPerTarget = 441;
    float alpha = 0.1f;
    float correlation = 0.0f;

    void fillBlock(audio_block_t *block) override {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        counterA++;
        if (counterA >= samplesPerTarget) {
          targetA = random(-32768, 32768) / 32768.0f;
          counterA = 0;
        }

        counterB++;
        if (counterB >= samplesPerTarget) {
          targetB = random(-32768, 32768) / 32768.0f;
          counterB = 0;
        }

        currentA = currentA * (1.0f - alpha) + targetA * alpha;
        currentB = currentB * (1.0f - alpha) + targetB * alpha;

        // NaN/Inf guard
        if (isnan(currentA) || isinf(currentA)) currentA = 0.0f;
        if (isnan(currentB) || isinf(currentB)) currentB = 0.0f;

        float blendedB = correlation * currentA + (1.0f - correlation) * currentB;
        float out = 0.5f * currentA + 0.5f * blendedB;

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

REGISTER_PLUGIN(TwinPulse);
