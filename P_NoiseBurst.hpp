// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"
#include "AudioStreamCustom.hpp"

class NoiseBurst : public Plugin {

public:

  NoiseBurst() {}

  ~NoiseBurst() override {}

  NoiseBurst(const NoiseBurst&) = delete;
  NoiseBurst& operator=(const NoiseBurst&) = delete;

  void init() override {
    customStream.burstCounter = 0;
    customStream.currentBurstLen = 200;
    customStream.burstProb = 0.001f;
  }

  void process(float k1, float k2) override {
    customStream.burstProb = 0.00002f + pow(k1, 2) * 0.003f;
    customStream.currentBurstLen = (int)(44 + k2 * 4400);
  }

  AudioStream& getStream() override { return customStream; }
  unsigned char getPort() override { return 0; }

private:

  class Stream : public AudioStreamCustom {
  public:
    int burstCounter = 0;
    int currentBurstLen = 200;
    float burstProb = 0.001f;

    void fillBlock(audio_block_t *block) override {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        // Generate white noise sample
        float noiseSample = random(-32768, 32768) / 32768.0f;

        if (burstCounter > 0) {
          // Envelope fade-out on last 20 samples
          float gain = 1.0f;
          if (burstCounter < 20) {
            gain = burstCounter / 20.0f;
          }
          int32_t out = (int32_t)(noiseSample * gain * 32767.0f);
          if (out > 32767) out = 32767;
          if (out < -32767) out = -32767;
          block->data[i] = (int16_t)out;
          burstCounter--;
        }
        else {
          if (random(0, 32768) / 32768.0f < burstProb) {
            burstCounter = currentBurstLen;
            // Output this sample as part of the burst
            float gain = 1.0f;
            if (burstCounter < 20) {
              gain = burstCounter / 20.0f;
            }
            int32_t out = (int32_t)(noiseSample * gain * 32767.0f);
            if (out > 32767) out = 32767;
            if (out < -32767) out = -32767;
            block->data[i] = (int16_t)out;
            burstCounter--;
          }
          else {
            block->data[i] = 0;
          }
        }
      }
    }
  } customStream;
};

REGISTER_PLUGIN(NoiseBurst);
