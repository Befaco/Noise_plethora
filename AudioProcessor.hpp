// Noise Plethora
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported
// See LICENSE.txt for the complete license text

#pragma once

#include <Audio.h>
#include <memory>

class PluginConnection {

  friend class AudioProcessor;

public:

  PluginConnection(std::unique_ptr<Plugin>& pin, AudioStream& output, unsigned char outPort)
    : plugin(std::move(pin))
    , patchCord(plugin->getStream(), plugin->getPort(), output, outPort)
  { plugin->init(); }

  ~PluginConnection() {}

  PluginConnection(const PluginConnection&) = delete;
  PluginConnection& operator=(const PluginConnection&) = delete;

private:

  void process(float a, float b) {
    if (plugin) {
      plugin->process(a, b);
    }
  }

  std::unique_ptr<Plugin> plugin;
  AudioConnection patchCord;

};

class AudioProcessor {

public:

  AudioProcessor()
    : chanA{ampA, 0, i2s1, 0}
    , chanB{ampB, 0, i2s1, 1}
  {};

  ~AudioProcessor() {};

  void initController() {
    sgtl5000_1.enable();   // Audio codec setup
    sgtl5000_1.lineOutLevel(10);
    sgtl5000_1.lineInLevel(0);
    sgtl5000_1.adcHighPassFilterDisable();
    sgtl5000_1.audioProcessorDisable();
    sgtl5000_1.autoVolumeDisable();

    ampA.gain(1.0);
    ampB.gain(1.0);
  }

  void processA(float a, float b) {
    if (currentPluginA) {
      currentPluginA->process(a, b);
    }
  }

  void processB(float a, float b) {
    if (currentPluginB) {
      currentPluginB->process(a, b);
    }
  }

  void setA(std::unique_ptr<Plugin>& pin, float gain = 1.0) {
    if (!pin) return;
    AudioNoInterrupts();
    currentPluginA = std::make_unique<PluginConnection>(pin, ampA, 0);
    ampA.gain(gain);
    AudioInterrupts();
  }

  void setB(std::unique_ptr<Plugin>& pin, float gain = 1.0) {
    if (!pin) return;
    AudioNoInterrupts();
    currentPluginB = std::make_unique<PluginConnection>(pin, ampB, 0);
    ampB.gain(gain);
    AudioInterrupts();
  }

private:

  AudioAmplifier                    ampA;
  AudioAmplifier                    ampB;
  AudioOutputI2S                    i2s1;
  AudioControlSGTL5000              sgtl5000_1;
  AudioConnection                   chanA;
  AudioConnection                   chanB;

  std::unique_ptr<PluginConnection> currentPluginA;
  std::unique_ptr<PluginConnection> currentPluginB;

};
