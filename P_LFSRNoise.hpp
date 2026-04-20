// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"
#include "AudioStreamCustom.hpp"

class LFSRNoise : public Plugin {

public:

  LFSRNoise() {}

  ~LFSRNoise() override {}

  LFSRNoise(const LFSRNoise&) = delete;
  LFSRNoise& operator=(const LFSRNoise&) = delete;

  void init() override {
    customStream.lfsr = 0xACE1;
    customStream.clockCounter = 0;
    customStream.samplesPerClock = 441;
    customStream.currentTaps = taps[0];
  }

  void process(float k1, float k2) override {
    float rate = 5.0f + pow(k1, 2) * 495.0f;
    int spc = (int)(44100.0f / rate);
    if (spc < 1) spc = 1;
    customStream.samplesPerClock = spc;

    int tapIndex = (int)(k2 * 7.99f);
    if (tapIndex < 0) tapIndex = 0;
    if (tapIndex > 7) tapIndex = 7;
    customStream.currentTaps = taps[tapIndex];
  }

  AudioStream& getStream() override { return customStream; }
  unsigned char getPort() override { return 0; }

private:

  static constexpr uint16_t taps[8] = {
    0x002D, 0x0039, 0xD008, 0xB400,
    0x6000, 0xD295, 0xB002, 0xE100
  };

  class Stream : public AudioStreamCustom {
  public:
    uint16_t lfsr = 0xACE1;
    int clockCounter = 0;
    int samplesPerClock = 441;
    uint16_t currentTaps = 0x002D;

    void fillBlock(audio_block_t *block) override {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        clockCounter++;
        if (clockCounter >= samplesPerClock) {
          uint16_t masked = lfsr & currentTaps;
          uint16_t feedback = __builtin_popcount(masked) & 1;
          lfsr = (lfsr >> 1) | (feedback << 15);
          if (lfsr == 0) lfsr = 0xACE1;  // prevent permanent lockup
          clockCounter = 0;
        }

        // Output top 4 bits: 16 discrete levels
        int16_t quantized = (int16_t)(lfsr >> 12);
        float out = quantized / 7.5f - 1.0f;

        // Add dither
        out += (random(0, 32768) / 32768.0f - 0.5f) * 0.01f;

        if (out > 1.0f) out = 1.0f;
        if (out < -1.0f) out = -1.0f;

        block->data[i] = (int16_t)(out * 32767.0f);
      }
    }
  } customStream;
};

REGISTER_PLUGIN(LFSRNoise);
