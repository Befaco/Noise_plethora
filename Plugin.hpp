// Noise Plethora
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported
// See LICENSE.txt for the complete license text

#pragma once

#include <Audio.h>
#include "PluginFactory.hpp" // Registrar is here

class Plugin {

public:

  Plugin() {}
  virtual ~Plugin() {}

  Plugin(const Plugin&) = delete;
  Plugin& operator=(const Plugin&) = delete;

  virtual void init() {};
  virtual void process(float k1, float k2) {};

  virtual AudioStream& getStream() { static AudioSynthWaveformDc unused; return unused; }
  virtual unsigned char getPort() { return 0; }

};

#define REGISTER_PLUGIN(NAME) \
    static Registrar<NAME> NAME ##_reg(#NAME)

