// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"
#include "AudioStreamCustom.hpp"

#define GLITCHLOOP_MAX_SAMPLES 8820

class GlitchLoop : public Plugin {

public:

  GlitchLoop() {}

  ~GlitchLoop() override {}

  GlitchLoop(const GlitchLoop&) = delete;
  GlitchLoop& operator=(const GlitchLoop&) = delete;

  void init() override {
    customStream.currentLoopLen = GLITCHLOOP_MAX_SAMPLES;
    customStream.loopPos = 0;
    customStream.bitDepthAccum = 16.0f;
    customStream.bitsToLose = 0.0f;

    // Fill buffer with white noise
    for (int i = 0; i < GLITCHLOOP_MAX_SAMPLES; i++) {
      customStream.loopBuffer[i] = random(-32768, 32768) / 32768.0f;
    }
  }

  void process(float k1, float k2) override {
    customStream.currentLoopLen = (int)(441.0f + pow(k1, 2.0f) * 8379.0f);
    if (customStream.currentLoopLen > GLITCHLOOP_MAX_SAMPLES) {
      customStream.currentLoopLen = GLITCHLOOP_MAX_SAMPLES;
    }
    if (customStream.currentLoopLen < 1) {
      customStream.currentLoopLen = 1;
    }

    customStream.bitsToLose = k2 * 0.05f;
  }

  AudioStream& getStream() override { return customStream; }
  unsigned char getPort() override { return 0; }

private:

  class Stream : public AudioStreamCustom {
  public:
    float loopBuffer[GLITCHLOOP_MAX_SAMPLES];
    int loopPos = 0;
    int currentLoopLen = GLITCHLOOP_MAX_SAMPLES;
    float bitDepthAccum = 16.0f;
    float bitsToLose = 0.0f;

    void fillBlock(audio_block_t *block) override {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        float sample = loopBuffer[loopPos];

        // Apply bit depth reduction
        int effectiveBits = (int)bitDepthAccum;
        if (effectiveBits < 1) effectiveBits = 1;
        if (effectiveBits > 16) effectiveBits = 16;

        // Bit crush: quantize to fewer bits
        int shift = 16 - effectiveBits;
        if (shift > 0) {
          int32_t intSample = (int32_t)(sample * 32767.0f);
          intSample = (intSample >> shift) << shift;
          sample = (float)intSample / 32767.0f;
        }

        // Write crushed sample back into buffer for progressive degradation
        loopBuffer[loopPos] = sample;

        int32_t out = (int32_t)(sample * 32767.0f);
        if (out > 32767) out = 32767;
        if (out < -32767) out = -32767;
        block->data[i] = (int16_t)out;

        loopPos++;
        if (loopPos >= currentLoopLen) {
          loopPos = 0;
          bitDepthAccum -= bitsToLose;

          // When bit depth is exhausted, refill with fresh noise
          if (bitDepthAccum < 1.0f) {
            bitDepthAccum = 16.0f;
            for (int j = 0; j < GLITCHLOOP_MAX_SAMPLES; j++) {
              loopBuffer[j] = random(-32768, 32768) / 32768.0f;
            }
          }
        }
      }
    }
  } customStream;
};

REGISTER_PLUGIN(GlitchLoop);
