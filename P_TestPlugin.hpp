// Noise Plethora Plugins
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under GPL-3.0-or-later

#pragma once

#include "Plugin.hpp"
#include "NoisePlethora.hpp"

class TestPlugin : public Plugin {

public:

  TestPlugin() {}

  ~TestPlugin() override {}

  TestPlugin(const TestPlugin&) = delete;
  TestPlugin& operator=(const TestPlugin&) = delete;

  void init() override {
    waveForm=WAVEFORM_SINE;

    waveform1.begin(waveForm);
    waveform1.amplitude(0.9);
  }

  void process(float k1, float k2) override {
    float knob_1 = k1;
    float knob_2 = k2;
    float pitch1 = pow(knob_1, 2);
    bool inA = g_process_mode == PROCESS_MODE_A;

    waveform1.frequency(20+(pitch1*1000));

    if (knob_2 <= 0.5) {
      waveForm = WAVEFORM_SINE;
    }
    else {
      waveForm = WAVEFORM_SQUARE;
    }
    waveform1.begin(waveForm);

    //testing pot & CV X
    Serial.print(inA ? "XA " : "XB ");
    if (knob_1 > 0.9) {
      Serial.print("Max OK");
    }
    else if (knob_1 < 0.1) {
      Serial.print("Min OK");
    }
    else {
      Serial.print("= ");
      Serial.print(knob_1);
    }
    Serial.print("       ");

    //testing pot & CV Y
    Serial.print(inA ? "YA " : "YB ");
    if (knob_2 > 0.9) {
      Serial.print("Max OK");
    }
    else if (knob_2 < 0.1) {
      Serial.print("Min OK");
    }
    else {
      Serial.print("= ");
      Serial.print(knob_2);
    }
    Serial.print("       ");

    //testing Program CV IN
    int analg = inA ? g_last_a2 : g_last_a1;
    Serial.print(inA ? "A2 " : "A1 ");
    if(analg > 1014) {
      Serial.print("PG_CV Min OK");
    }
    else if (analg < 10) {
      Serial.print("PG_CV Max OK");
    }
    else {
      Serial.print("PG_CV = ");
      Serial.print(analg);
    }
    if (g_process_mode == PROCESS_MODE_A) {
      Serial.print("       ");
    }
    else {
      Serial.println();
    }
  }

  AudioStream& getStream() override { return waveform1; }
  unsigned char getPort() override { return 0; }

private:

  AudioSynthWaveform       waveform1;
  int                      waveForm;

};

REGISTER_PLUGIN(TestPlugin);
