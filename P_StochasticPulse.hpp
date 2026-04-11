// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"
#include "AudioStreamCustom.hpp"

class StochasticPulse : public Plugin {

public:

  StochasticPulse() {}

  ~StochasticPulse() override {}

  StochasticPulse(const StochasticPulse&) = delete;
  StochasticPulse& operator=(const StochasticPulse&) = delete;

  void init() override {
    customStream.density = 50.0f;
    customStream.lastPulseSign = false;
    customStream.dryMix = 0.3f;
    customStream.filterFreq = 1000.0f;
    customStream.filterQ = 3.5f;

    // Init SVF state
    customStream.svfLow = 0.0f;
    customStream.svfBand = 0.0f;
  }

  void process(float k1, float k2) override {
    customStream.density = 5.0f + pow(k1, 2) * 500.0f;
    customStream.filterFreq = 80.0f + pow(k2, 2) * 8000.0f;
  }

  AudioStream& getStream() override { return customStream; }
  unsigned char getPort() override { return 0; }

private:

  class Stream : public AudioStreamCustom {
  public:
    float density = 50.0f;
    float dryMix = 0.3f;
    bool lastPulseSign = false;
    float filterFreq = 1000.0f;
    float filterQ = 3.5f;

    // State variable filter state
    float svfLow = 0.0f;
    float svfBand = 0.0f;

    void fillBlock(audio_block_t *block) override {
      float probability = density / 44100.0f;

      // SVF coefficients
      float f = 2.0f * sin(M_PI * filterFreq / 44100.0f);
      float q = 1.0f / filterQ;

      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        // Generate stochastic impulse
        float impulse = 0.0f;
        if (random(0, 32768) / 32768.0f < probability) {
          lastPulseSign = !lastPulseSign;
          impulse = lastPulseSign ? 1.0f : -1.0f;
        }

        // State variable filter (bandpass)
        svfLow += f * svfBand;
        float svfHigh = impulse - svfLow - q * svfBand;
        svfBand += f * svfHigh;

        // Clamp SVF state to prevent blowup
        if (isnan(svfBand) || isinf(svfBand)) svfBand = 0.0f;
        if (isnan(svfLow) || isinf(svfLow)) svfLow = 0.0f;

        // Mix: dry click + resonant ring (bandpass output)
        float mixed = impulse * dryMix + svfBand;

        if (mixed > 1.0f) mixed = 1.0f;
        if (mixed < -1.0f) mixed = -1.0f;
        block->data[i] = (int16_t)(mixed * 32767.0f);
      }
    }
  } customStream;
};

REGISTER_PLUGIN(StochasticPulse);
