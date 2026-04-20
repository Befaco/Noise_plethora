// AudioStreamCustom: Base class for plugins that implement per-sample DSP
// instead of using the Teensy Audio Library's AudioConnection graph.
//
// Subclasses implement fillBlock() to generate 128 samples of audio.
// The Teensy audio engine calls update() automatically at block rate,
// which allocates a block, calls fillBlock(), and transmits the result.

#pragma once

#include <Audio.h>

class AudioStreamCustom : public AudioStream {
public:
  AudioStreamCustom() : AudioStream(0, NULL) {}
  virtual ~AudioStreamCustom() {}

  // Subclass implements this: fill block->data[0..AUDIO_BLOCK_SAMPLES-1]
  // with int16_t samples. Called once per audio block (~2.9ms at 44.1kHz).
  virtual void fillBlock(audio_block_t *block) = 0;

  void update() override {
    audio_block_t *block = allocate();
    if (block) {
      fillBlock(block);
      transmit(block);
      release(block);
    }
  }
};
