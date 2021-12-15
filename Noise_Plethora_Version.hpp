// Noise Plethora
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported
// See LICENSE.txt for the complete license text

#pragma once

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define NP_VERSION_MAJOR 1
#define NP_VERSION_MINOR 5

#define NP_VERSION_STRING STR(NP_VERSION_MAJOR) "." STR(NP_VERSION_MINOR)
