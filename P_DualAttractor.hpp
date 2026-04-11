// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"
#include "AudioStreamCustom.hpp"

class DualAttractor : public Plugin {

public:

  DualAttractor() {}

  ~DualAttractor() override {}

  DualAttractor(const DualAttractor&) = delete;
  DualAttractor& operator=(const DualAttractor&) = delete;

  void init() override {
    // Lorenz system 1
    x1 = 1.0f;
    y1 = 1.0f;
    z1 = 1.0f;

    // Lorenz system 2
    x2 = -1.0f;
    y2 = 0.0f;
    z2 = 2.0f;

    coupling = 0.0f;
    baseFreq = 100.0f;

    customStream.sine1Freq = 100.0f;
    customStream.sine2Freq = 150.0f;
    customStream.sine1Phase = 0.0f;
    customStream.sine2Phase = 0.0f;
  }

  void process(float k1, float k2) override {
    coupling = k1 * 5.0f;
    baseFreq = 30.0f + pow(k2, 2.0f) * 2000.0f;

    // Lorenz parameters
    const float sigma = 10.0f;
    const float rho = 28.0f;
    const float beta = 8.0f / 3.0f;
    const float dt = 0.001f;

    // Run 10 integration steps for stability
    for (int step = 0; step < 10; step++) {
      float dx1 = sigma * (y1 - x1) + coupling * (x2 - x1);
      float dy1 = x1 * (rho - z1) - y1;
      float dz1 = x1 * y1 - beta * z1;

      float dx2 = sigma * (y2 - x2) + coupling * (x1 - x2);
      float dy2 = x2 * (rho - z2) - y2;
      float dz2 = x2 * y2 - beta * z2;

      x1 += dx1 * dt;
      y1 += dy1 * dt;
      z1 += dz1 * dt;

      x2 += dx2 * dt;
      y2 += dy2 * dt;
      z2 += dz2 * dt;
    }

    // Check for NaN/infinity and reset if needed
    if (isnan(x1) || isinf(x1) || isnan(y1) || isinf(y1) || isnan(z1) || isinf(z1)) {
      x1 = 1.0f;
      y1 = 1.0f;
      z1 = 1.0f;
    }
    if (isnan(x2) || isinf(x2) || isnan(y2) || isinf(y2) || isnan(z2) || isinf(z2)) {
      x2 = -1.0f;
      y2 = 0.0f;
      z2 = 2.0f;
    }

    // Map Lorenz x outputs to frequency offsets
    float freq1 = baseFreq + x1 * baseFreq * 0.4f;
    float freq2 = baseFreq * 1.5f + x2 * baseFreq * 0.4f;

    // Clamp frequencies to reasonable range
    if (freq1 < 20.0f) freq1 = 20.0f;
    if (freq1 > 20000.0f) freq1 = 20000.0f;
    if (freq2 < 20.0f) freq2 = 20.0f;
    if (freq2 > 20000.0f) freq2 = 20000.0f;

    customStream.sine1Freq = freq1;
    customStream.sine2Freq = freq2;
  }

  AudioStream& getStream() override { return customStream; }
  unsigned char getPort() override { return 0; }

private:

  // Lorenz system 1
  float x1 = 1.0f;
  float y1 = 1.0f;
  float z1 = 1.0f;

  // Lorenz system 2
  float x2 = -1.0f;
  float y2 = 0.0f;
  float z2 = 2.0f;

  float coupling = 0.0f;
  float baseFreq = 100.0f;

  class Stream : public AudioStreamCustom {
  public:
    float sine1Freq = 100.0f;
    float sine2Freq = 150.0f;
    float sine1Phase = 0.0f;
    float sine2Phase = 0.0f;

    void fillBlock(audio_block_t *block) override {
      const float twoPi = 2.0f * 3.14159265358979323846f;

      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        // Sine oscillator 1
        sine1Phase += twoPi * sine1Freq / 44100.0f;
        if (sine1Phase >= twoPi) sine1Phase -= twoPi;
        float s1 = sin(sine1Phase) * 0.7f;

        // Sine oscillator 2
        sine2Phase += twoPi * sine2Freq / 44100.0f;
        if (sine2Phase >= twoPi) sine2Phase -= twoPi;
        float s2 = sin(sine2Phase) * 0.7f;

        // Mix both sines
        float mixed = s1 + s2;

        if (mixed > 1.0f) mixed = 1.0f;
        if (mixed < -1.0f) mixed = -1.0f;
        block->data[i] = (int16_t)(mixed * 32767.0f);
      }
    }
  } customStream;
};

REGISTER_PLUGIN(DualAttractor);
