// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"

class RunawayFilter : public Plugin {

public:

  RunawayFilter()
    : patchCord1(noise1, 0, filter1, 0)
    { }

  ~RunawayFilter() override {}

  RunawayFilter(const RunawayFilter&) = delete;
  RunawayFilter& operator=(const RunawayFilter&) = delete;

  void init() override {
    noise1.amplitude(0.5f);
    filter1.frequency(1000.0f);
    filter1.resonance(4.8f);
    filter1.octaveControl(3.0f);
  }

  void process(float k1, float k2) override {
    float freq = 30.0f + pow(k1, 2.0f) * 10000.0f;
    float noiseAmp = 0.01f + k2 * 0.99f;
    float resonance = 4.5f + (1.0f - k2) * 0.49f;

    noise1.amplitude(noiseAmp);
    filter1.frequency(freq);
    filter1.resonance(resonance);
  }

  AudioStream& getStream() override {
    return filter1;
  }
  unsigned char getPort() override {
    return 1;
  }

private:
  AudioSynthNoiseWhite     noise1;
  AudioFilterStateVariable filter1;
  AudioConnection          patchCord1;
};

REGISTER_PLUGIN(RunawayFilter);
