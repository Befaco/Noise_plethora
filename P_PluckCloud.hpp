// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"
#include "AudioStreamCustom.hpp"

class PluckCloud : public Plugin {

public:

  PluckCloud() {}

  ~PluckCloud() override {}

  PluckCloud(const PluckCloud&) = delete;
  PluckCloud& operator=(const PluckCloud&) = delete;

  void init() override {
    for (int v = 0; v < 6; v++) {
      customStream.writePos[v] = 0;
      customStream.filterState[v] = 0.0f;
      customStream.triggered[v] = false;
      for (int i = 0; i < 1024; i++) {
        customStream.buf[v][i] = 0.0f;
      }
    }
    customStream.damping = 0.5f;
    customStream.baseDelay = 100.0f;
  }

  void process(float k1, float k2) override {
    float freq = 60.0f + pow(k1, 2) * 2940.0f;
    customStream.baseDelay = 44100.0f / freq;
    customStream.damping = 0.98f - k2 * 0.58f;

    // random retrigger: each voice has ~2% chance per process() call
    for (int v = 0; v < 6; v++) {
      if (random(0, 32768) / 32768.0f < 0.02f) {
        triggerVoice(v);
      }
    }
  }

  AudioStream& getStream() override { return customStream; }
  unsigned char getPort() override { return 0; }

private:

  void triggerVoice(int v) {
    float voiceDelay = customStream.baseDelay;
    int delay = (int)voiceDelay;
    if (delay < 1) delay = 1;
    if (delay > 1023) delay = 1023;

    for (int i = 0; i < delay; i++) {
      int pos = (customStream.writePos[v] - delay + i + 1024) & 1023;
      customStream.buf[v][pos] = random(-32768, 32768) / 32768.0f;
    }
    customStream.filterState[v] = 0.0f;
    customStream.triggered[v] = true;
  }

  class Stream : public AudioStreamCustom {
  public:
    float buf[6][1024] = {};
    int writePos[6] = {};
    float filterState[6] = {};
    bool triggered[6] = {};
    float damping = 0.5f;
    float baseDelay = 100.0f;

    void fillBlock(audio_block_t *block) override {
      // voice detune ratios
      static const float detuneRatio[6] = {1.0f, 1.005f, 0.995f, 1.01f, 0.99f, 1.015f};

      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        float mixed = 0.0f;

        for (int v = 0; v < 6; v++) {
          float voiceDelay = baseDelay * detuneRatio[v];
          int delay = (int)voiceDelay;
          if (delay < 1) delay = 1;
          if (delay > 1023) delay = 1023;

          int readPos = writePos[v] - delay;
          if (readPos < 0) readPos += 1024;

          // read from delay and apply one-pole LP filter
          float delayOut = buf[v][readPos];
          float filtered = damping * filterState[v] + (1.0f - damping) * delayOut;
          filterState[v] = filtered;

          // write filtered output back into delay line
          buf[v][writePos[v]] = filtered;
          writePos[v] = (writePos[v] + 1) & 1023; // mod 1024

          mixed += filtered;
        }

        // scale by number of voices
        mixed *= (1.0f / 6.0f);

        // clamp and convert to int16
        if (mixed > 1.0f) mixed = 1.0f;
        if (mixed < -1.0f) mixed = -1.0f;
        block->data[i] = (int16_t)(mixed * 32767.0f);
      }
    }
  } customStream;
};

REGISTER_PLUGIN(PluckCloud);
