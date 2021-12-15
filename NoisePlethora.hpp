// Noise Plethora
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported
// See LICENSE.txt for the complete license text

#pragma once

enum {
  PROCESS_MODE_NONE = -1,
  PROCESS_MODE_A = 0,
  PROCESS_MODE_B = 1
};
extern int g_process_mode;

extern uint16_t g_last_a2;
extern uint16_t g_last_a1;
