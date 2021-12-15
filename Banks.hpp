// Noise Plethora
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported
// See LICENSE.txt for the complete license text

#pragma once

#include <string>
#include <memory>

static const int programsPerBank = 10;
static const int numBanks = 5;

struct Bank {

  struct BankElem {
    BankElem() {};

    BankElem(std::string n, float g = 1.0)
    : name{n}
    , gain{g}
    {}

    std::string name = "";
    float gain = 1.0;
  };

  static const BankElem defaultElem;

  Bank();
  Bank(const BankElem& p1, const BankElem& p2 = defaultElem,
       const BankElem& p3 = defaultElem, const BankElem& p4 = defaultElem,
       const BankElem& p5 = defaultElem, const BankElem& p6 = defaultElem,
       const BankElem& p7 = defaultElem, const BankElem& p8 = defaultElem,
       const BankElem& p9 = defaultElem, const BankElem& p10 = defaultElem);

  const char* getProgramName(int i);
  float getProgramGain(int i);

  int getSize();

private:

  std::array<BankElem, programsPerBank> programs;

};

Bank& getBankForIndex(int i);
