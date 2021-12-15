// Noise Plethora
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported
// See LICENSE.txt for the complete license text

#include "QuadEncoder.h"  //Library info: https://github.com/mjs513/Teensy-4.x-Quad-Encoder-Library
#include "SevSeg.h"       //library info: https://github.com/DeanIsMe/SevSeg
#include <Audio.h>        //library info: https://www.pjrc.com/teensy/gui/index.html?info=AudioControlSGTL5000

#include <Wire.h>
#include <SPI.h>
#include <Bounce.h>
#include <EEPROM.h>

#include <cstdlib>
#include <ctime>

#include "Noise_Plethora_Version.hpp"
#include "WhiteNoise.hpp"

#include "Plugin.hpp"

#include "AudioProcessor.hpp"
#include "ProgramSelector.hpp"

// required by the TestPlugin
#include "NoisePlethora.hpp"
int g_process_mode = PROCESS_MODE_NONE;
uint16_t g_last_a2 = 0;
uint16_t g_last_a1 = 0;

IntervalTimer noiseTimer;
IntervalTimer displayTimer;

Bounce button0 = Bounce(2, 15);

QuadEncoder encoder(1, 0, 1, 1); // Quadrature Decoder Number(1), PhaseA(pin0), PhaseB(pin1), Pullups Req(1)

// #define DEBUGGING // enable to get some serial port logging

#define CV_ADC_MIDPOINT 511
#define CV_HYSTERESIS 10
#define CV_SETTLE_THRESH 5

SevSeg sevseg;

const int Disp1 = 33;
const int Disp2 = 32;
const int ledPinBANK = 30;

long positionEncoder = -999;
long buttonStartTime = 0;
long testModeButtonStartTime = 0;
bool inBankMode = false;
bool actionOnButtonUp = true;
bool wantsLoadTestMode = false;
bool wantsUnloadTestMode = false;
bool inTestMode = false;

AudioProcessor processor;
ProgramSelector program;

long last_pA = 512;
long last_pB = 512;

// using ADC lib for better control
#include <ADC.h>
#include <ADC_util.h>
ADC *adc = new ADC(); // adc object

const byte state_version = 1;
byte state_progCVProgramEverywhere = 0;
byte state_progA_Bank = 0;
byte state_progA_Program = 0;
byte state_progB_Bank = 0;
byte state_progB_Program = 0;

unsigned long startupTimer = 0;

int bankSizes[numBanks];
int totalPrograms;
struct CVModData {
  int bank;
  int prog;
  float cvnorm;
  int numProgramsInCurrentBank;
  ProgramSelection& selection;
};

// #include <T4_PowerButton.h>
// unsigned long ramTimer = 0;
//
// void reportRam() {
//   unsigned long ms = millis();
//   if (!ramTimer || ms - ramTimer > 2000) {
//     Serial.print("AudioMemoryUsage: ");
//     Serial.println(AudioMemoryUsage());
//     Serial.print("AudioMemoryUsageMax: ");
//     Serial.println(AudioMemoryUsageMax());
//     Serial.print("memfree: ");
//     Serial.println(memfree());
//     Serial.print("heapfree: ");
//     Serial.println(heapfree());
//     Serial.print("maxstack: ");
//     Serial.println(maxstack());
//     flexRamInfo();
//     ramTimer = ms;
//   }
// }

void setup() {

  Serial.begin(9600);

  ///// ADC0 /////
  adc->adc0->setAveraging(16);                                    // set number of averages
  adc->adc0->setResolution(10);                                   // set bits of resolution
  adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED); // change the conversion speed
  adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);     // change the sampling speed

  ///// ADC1 /////
  adc->adc1->setAveraging(16);                                    // set number of averages
  adc->adc1->setResolution(10);                                   // set bits of resolution
  adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED); // change the conversion speed
  adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);     // change the sampling speed

  setupNoise();
  noiseTimer.begin(loopNoise, 10);

  pinMode(2, INPUT_PULLUP); //pull up for encoder button

  AudioMemory(200); //connections require memory to work. More info at "MemoryAndCpuUsage" example

  processor.initController();

  encoder.setInitConfig(); // Initialize Encoder
  encoder.EncConfig.filterCount = 1; // Debouncing filter setup, range 0-7
  encoder.EncConfig.filterSamplePeriod = 100; // Debouncing filter setup, range 0-255

  encoder.init();

  pinMode(Disp1, OUTPUT); // set the displays A and B common pins and Bank LED as DIgital Outs
  pinMode(Disp2, OUTPUT);
  pinMode(ledPinBANK, OUTPUT);

  // configure the displays
  byte numDigits = 2;
  byte digitPins[] = { Disp1, Disp2 };
  byte segmentPins[] = {3, 4, 5, 6, 9, 10, 11, 17};
  bool resistorsOnSegments = true;
  char updateWithDelays = false; // default
  char leadingZeros = true;
  byte hardwareConfig = COMMON_ANODE;
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros);
  sevseg.setBrightness(50);

  startupTimer = millis();

  digitalWrite(ledPinBANK, inBankMode ? HIGH : LOW);

  button0.update(); // initial update

  // init the position
  positionEncoder = encoder.read() / 2;

  totalPrograms = 0;
  for (int i = 0; i < numBanks; i++) {
    bankSizes[i] = getBankForIndex(i).getSize();
    totalPrograms += bankSizes[i];
  }

  byte vers = EEPROM.read(0);
  if (vers != state_version) {
    Serial.print("initializing EEPROM, version ");
    Serial.println(state_version);
    EEPROM.write(0, state_version); // initialize EEPROM, version + 16 bytes
    for (int i = 1; i < 17; i++) {
      EEPROM.write(i, 0);
    }
  }
  else {
    Serial.println("restoring EEPROM values");
    state_progCVProgramEverywhere  = EEPROM.read(1);
    // leave some space, maybe we need some flags for something
    state_progA_Bank      = EEPROM.read(4);
    state_progA_Program   = EEPROM.read(5);
    program.getA().setBank(state_progA_Bank);
    program.getA().setProgram(state_progA_Program);

    state_progB_Bank      = EEPROM.read(6);
    state_progB_Program   = EEPROM.read(7);
    program.getB().setBank(state_progB_Bank);
    program.getB().setProgram(state_progB_Program);
  }

  auto nameA = program.getA().getCurrentProgramName();
  auto gainA = program.getA().getCurrentProgramGain();
  auto pinA = PluginFactory::Instance()->Create(nameA);
  processor.setA(pinA, gainA);

  auto nameB = program.getB().getCurrentProgramName();
  auto gainB = program.getB().getCurrentProgramGain();
  auto pinB = PluginFactory::Instance()->Create(nameB);
  processor.setB(pinB, gainB);

  displayTimer.begin(loopDisplay, 500);
}

long progA_last = 0;
long progB_last = 0;
bool firstLoop = true;
long stateTimer = 0;
long stateTimerA = 0;
long stateTimerB = 0;
long bankOnlyBlinker = 0;
long wasBankOnlyBlinker = 0;

int settleCountA = -1;
int settleCountB = -1;

void loop() {

  unsigned long ms = millis();

  button0.update(); //Read encoder button

  handleStartup(ms); // deal with CV mode change/display

  // Read CV and knobs,sum them and scale to 0-1.0
  float knob_1 = (float)1.0 - adc->analogRead(A10) / 1023.0;
  float knob_2 = (float)1.0 - adc->analogRead(A11) / 1023.0;
  float knob_3 = (float)1.0 - adc->analogRead(A12) / 1023.0;
  float knob_4 = (float)1.0 - adc->analogRead(A13) / 1023.0;

  bool checkCVA = false, checkCVB = false;

  ADC::Sync_result sr = adc->analogSynchronizedRead(A2, A1);
  g_last_a2 = sr.result_adc0 = (uint16_t)sr.result_adc0;
  g_last_a1 = sr.result_adc1 = (uint16_t)sr.result_adc1;

  float progA_in = 1023.0 - sr.result_adc0;
  float progB_in = 1023.0 - sr.result_adc1;

  float progA_norm = ((progA_in / 1023.0) * 2.) - 1.;
  float progB_norm = ((progB_in / 1023.0) * 2.) - 1.;

  // only apply CV if the value has changed more than 10 in either direction
  if (abs(progA_in - last_pA) > CV_HYSTERESIS) {
#ifdef DEBUGGING
    Serial.print("CV A registered: ");
    Serial.println(progA_in);
#endif
    last_pA = progA_in;
    //checkCVA = true;
    settleCountA = 0;
  }
  else if (settleCountA >= 0) {
    last_pA = progA_in;
    if (++settleCountA > CV_SETTLE_THRESH) {
      settleCountA = -1;
      checkCVA = true;
#ifdef DEBUGGING
      Serial.print("progA changed: ");
      Serial.print(progA_in);
      Serial.print(" . Norm: ");
      Serial.println(progA_norm);
#endif
    }
  }
  if (abs(progB_in - last_pB) > CV_HYSTERESIS) {
#ifdef DEBUGGING
    Serial.print("CV B registered: ");
    Serial.println(progB_in);
#endif
    last_pB = progB_in;
    // checkCVB = true;
    settleCountB = 0;
  }
  else if (settleCountB >= 0){
    last_pB = progB_in;
    if (++settleCountB > CV_SETTLE_THRESH) {
      settleCountB = -1;
      checkCVB = true;
#ifdef DEBUGGING
      Serial.print("progB changed: ");
      Serial.print(progB_in);
      Serial.print(" . Norm: ");
      Serial.println(progB_norm);
#endif
    }
  }

  if (inTestMode) {
    checkCVA = checkCVB = false; // completely disable CV and program changing in test mode
  }

  handleBankMode(ms);

  bool switchProgramA = false;
  bool switchProgramB = false;

  // NVIC_DISABLE_IRQ(noiseTimer);
  long newLeft = encoder.read() / 4; //encoder clicks divided by two to adapt to this encoder model
  // NVIC_ENABLE_IRQ(noiseTimer);

  long delta = newLeft - positionEncoder;
  bool modeA = (program.getMode() == PROG_A);

  if (!inTestMode && newLeft != positionEncoder) {
    positionEncoder = newLeft;
    ProgramSelection& sel = program.getCurrent();
    if (inBankMode) {
      int prevBank = modeA ? state_progA_Bank : state_progB_Bank;
      int newBank = sel.setBank(prevBank + delta);
      if (newBank != prevBank) {
        if (modeA) {
#ifdef DEBUGGING
          Serial.print("updating bank A state: ");
          Serial.println(newBank);
#endif
          state_progA_Bank = newBank;
          state_progA_Program = sel.setProgram(state_progA_Program); // ensure range
          switchProgramA = true;
          stateTimerA = stateTimer = ms;
        }
        else {
#ifdef DEBUGGING
          Serial.print("updating bank B state: ");
          Serial.println(newBank);
#endif
          state_progB_Bank = newBank;
          state_progB_Program = sel.setProgram(state_progB_Program); // ensure range
          switchProgramB = true;
          stateTimerB = stateTimer = ms;
        }
      }
    }
    else {
      int prevProg = modeA ? state_progA_Program : state_progB_Program;
      int newProg = sel.setProgram(prevProg + delta);
      if (newProg != prevProg) {
        if (modeA) {
#ifdef DEBUGGING
          Serial.print("updating prog A state: ");
          Serial.println(newProg);
#endif
          state_progA_Program = newProg;
          switchProgramA = true;
          stateTimerA = stateTimer = ms;
        }
        else {
#ifdef DEBUGGING
          Serial.print("updating prog B state: ");
          Serial.println(newProg);
#endif
          state_progB_Program = newProg;
          switchProgramB = true;
          stateTimerB = stateTimer = ms;
        }
      }
    }
  }

  if (!inTestMode && stateTimer && ms - stateTimer > 2000) {
    Serial.println("writing EEPROM");
    EEPROM.write(1, state_progCVProgramEverywhere);
    EEPROM.write(4, state_progA_Bank);
    EEPROM.write(5, state_progA_Program);
    EEPROM.write(6, state_progB_Bank);
    EEPROM.write(7, state_progB_Program);
    stateTimer = 0;
  }

  // now apply cv
  long valA, valB;
  long newBankA, newProgA;
  long newBankB, newProgB;

  int numProgramsInCurrentBankA = bankSizes[state_progA_Bank];
  int numProgramsInCurrentBankB = bankSizes[state_progB_Bank];

  ProgramSelection programTempA(program.getA());
  if (checkCVA) {
    CVModData cvA = {
      state_progA_Bank,
      state_progA_Program,
      progA_norm,
      numProgramsInCurrentBankA,
      programTempA
    };
    handleCV(cvA);
  }
  newBankA = programTempA.getBank();
  newProgA = programTempA.getProgram();
  if (checkCVA && (program.getA().getBank() != newBankA
      || program.getA().getProgram() != newProgA))
  {
#ifdef DEBUGGING
    Serial.print("switching A to: b");
    Serial.print(newBankA);
    Serial.print(" / p");
    Serial.println(newProgA);
#endif
    program.getA().setBank(newBankA);
    program.getA().setProgram(newProgA);
    switchProgramA = true;
  }
#ifdef DEBUGGING
  else if (checkCVA) {
    Serial.print("program.getA().getBank(): ");
    Serial.print(program.getA().getBank());
    Serial.print(", state_progA_Bank: ");
    Serial.print(state_progA_Bank);
    Serial.print(", newBankA: ");
    Serial.print(newBankA);
    Serial.print(", program.getA().getProgram(): ");
    Serial.print(program.getA().getProgram());
    Serial.print(", state_progA_Program: ");
    Serial.print(state_progA_Program);
    Serial.print(", newProgA: ");
    Serial.println(newProgA);
  }
#endif

  ProgramSelection programTempB(program.getB());
  if (checkCVB) {
    CVModData cvB = {
      state_progB_Bank,
      state_progB_Program,
      progB_norm,
      numProgramsInCurrentBankB,
      programTempB
    };
    handleCV(cvB);
  }
  newBankB = programTempB.getBank();
  newProgB = programTempB.getProgram();
  if (checkCVB && (program.getB().getBank() != newBankB
      || program.getB().getProgram() != newProgB))
  {
#ifdef DEBUGGING
    Serial.print("switching B to: b");
    Serial.print(newBankB);
    Serial.print(" / p");
    Serial.println(newProgB);
#endif
    program.getB().setBank(newBankB);
    program.getB().setProgram(newProgB);
    switchProgramB = true;
  }
#ifdef DEBUGGING
  else if (checkCVB) {
    Serial.print("program.getB().getBank(): ");
    Serial.print(program.getB().getBank());
    Serial.print(", state_progB_Bank: ");
    Serial.print(state_progB_Bank);
    Serial.print(", newBankB: ");
    Serial.print(newBankB);
    Serial.print(", program.getB().getProgram(): ");
    Serial.print(program.getB().getProgram());
    Serial.print(", state_progB_Program: ");
    Serial.print(state_progB_Program);
    Serial.print(", newProgB: ");
    Serial.println(newProgB);
  }
#endif

  valA = inBankMode ? newBankA : newProgA;
  valB = inBankMode ? newBankB : newProgB;

  if (wantsLoadTestMode) {
    auto pinA = PluginFactory::Instance()->Create("TestPlugin");
    processor.setA(pinA);
    auto pinB = PluginFactory::Instance()->Create("TestPlugin");
    processor.setB(pinB);
    wantsLoadTestMode = false;
    inTestMode = true;
  }
  else {
    if (wantsUnloadTestMode) {
      switchProgramA = switchProgramB = true;
      wantsUnloadTestMode = false;
      inTestMode = false;
    }

    if (switchProgramA) {
      auto nameA = program.getA().getCurrentProgramName();
      auto gainA = program.getA().getCurrentProgramGain();
#ifdef DEBUGGING
      Serial.print("bank A, switching to: ");
      Serial.print(nameA);
      Serial.print(" (");
      Serial.print(program.getA().getProgram());
      Serial.println(")");
#endif
      auto pinA = PluginFactory::Instance()->Create(nameA);
      processor.setA(pinA, gainA);
    }
    if (switchProgramB) {
      auto nameB = program.getB().getCurrentProgramName();
      auto gainB = program.getB().getCurrentProgramGain();
#ifdef DEBUGGING
      Serial.print("bank B, switching to: ");
      Serial.print(nameB);
      Serial.print(" (");
      Serial.print(program.getB().getProgram());
      Serial.println(")");
#endif
      auto pinB = PluginFactory::Instance()->Create(nameB);
      processor.setB(pinB, gainB);
    }
  }

  // display handling
  if (ms - startupTimer < 2000) {
    sevseg.setChars(NP_VERSION_STRING);
  }
  else if (inTestMode) {
    char charsTestMode[] = "8.8.";
    sevseg.setChars(charsTestMode);
  }
  else {
    int valToDisplayA = valA;
    int valToDisplayB = valB;
    if (stateTimer && ms - stateTimer <= 2000) {
      if (stateTimerA) {
        if (ms - stateTimerA <= 2000) {
          valToDisplayA = inBankMode ? state_progA_Bank : state_progA_Program;
        }
        else {
          stateTimerA = 0;
        }
      }
      if (stateTimerB) {
        if (ms - stateTimerB <= 2000) {
          valToDisplayB = inBankMode ? state_progB_Bank : state_progB_Program;
        }
        else {
          stateTimerB = 0;
        }
      }
    }
    if (inBankMode) {
      static char charset[] = "AbCdEFHIJLnoPrSUYZ";
      char charsToDisplay[] = "A.A";
      charsToDisplay[0] = charset[valToDisplayA];
      charsToDisplay[1] = (modeA) ? '.' : charset[valToDisplayB];
      charsToDisplay[2] = (modeA) ? charset[valToDisplayB] : '.';
      sevseg.setChars(charsToDisplay);
    }
    else {
      float numberToDisplay = (valToDisplayA * 10) + valToDisplayB;
      int decimalPoint = 0;
      if (modeA) {
        decimalPoint = 1;
      }
      sevseg.setNumber(numberToDisplay, decimalPoint);
    }
  }
  // display refreshed in interrupt (displayTimer)

  if (!inTestMode && button0.risingEdge()) {
    if (bankOnlyBlinker || wasBankOnlyBlinker) {
      bankOnlyBlinker = wasBankOnlyBlinker = 0;
    }
    else {
      if (actionOnButtonUp) {
        program.setMode(!program.getMode());
      }
    }
    actionOnButtonUp = true;
    buttonStartTime = testModeButtonStartTime = 0;
  }

  AudioNoInterrupts();
  g_process_mode = PROCESS_MODE_A;
  processor.processA(knob_1, knob_2);
  g_process_mode = PROCESS_MODE_B;
  processor.processB(knob_3, knob_4);
  g_process_mode = PROCESS_MODE_NONE;
  AudioInterrupts();
}


void handleStartup(long ms) {
  static bool bankOnlySet = false;
  static bool wasDown = false;
  static long bankOnlyCount = 0;

  bool down = !button0.read();

  if (firstLoop) {
    bankOnlyBlinker = ms;
    firstLoop = false;
  }

  if (bankOnlyBlinker) {
    long dur = ms - bankOnlyBlinker;

    wasDown |= down;

    if (down) {
      if (dur >= 11000) { // normal behavior, display bank mode
        digitalWrite(ledPinBANK, inBankMode ? HIGH : LOW);
        bankOnlyBlinker = 0;
        wasBankOnlyBlinker = 1;
      }
      else if (dur >= 10000 && !bankOnlySet) { // 10s to change the state, hidden
        state_progCVProgramEverywhere = !state_progCVProgramEverywhere;
        Serial.print("updating CV program everywhere state: ");
        Serial.println(state_progCVProgramEverywhere);
        stateTimer = ms;
        bankOnlySet = true;
      }
      else if (dur >= 8000) { // show the current state
        digitalWrite(ledPinBANK, state_progCVProgramEverywhere);
      }
      else { // blink to indicate that we're in some special mode
        long count = dur / (dur >= 4000 ? 100 : 200);
        if (count > bankOnlyCount) {
          bankOnlyCount = count;
          digitalWrite(ledPinBANK, bankOnlyCount % 2);
        }
      }
    }
    else {
      if (dur >= 2000) {
        digitalWrite(ledPinBANK, inBankMode ? HIGH : LOW);
        bankOnlyBlinker = 0; // abort, not changing the state
      }
      else { // show the current state
        digitalWrite(ledPinBANK, state_progCVProgramEverywhere);
      }
      wasBankOnlyBlinker = wasDown ? 1 : 0;
    }
  }
}

bool handleBankMode(long ms) {
  if (!bankOnlyBlinker) {
    if (button0.fallingEdge()) {
      buttonStartTime = testModeButtonStartTime = ms;
    }
    else if (buttonStartTime
             && ms - buttonStartTime > 400)  // hold time to get in to bank mode
    {
      if (inTestMode) {
        if (ms - buttonStartTime > 2000) { // 2s to get out of test mode
          wantsUnloadTestMode = true;
          actionOnButtonUp = false;
          inBankMode = !inBankMode; // reverse whatever happened to get here
          digitalWrite(ledPinBANK, inBankMode ? HIGH : LOW);
          buttonStartTime = testModeButtonStartTime = 0;
        }
        return true;
      }
      inBankMode = !inBankMode;
      digitalWrite(ledPinBANK, inBankMode ? HIGH : LOW);
      actionOnButtonUp = false;
      buttonStartTime = 0;
      return true;
    }
    else if (testModeButtonStartTime
             && ms - testModeButtonStartTime > 10000)
    {
      wantsLoadTestMode = true;
      actionOnButtonUp = false;
      buttonStartTime = testModeButtonStartTime = 0;
      return true;
    }
    else if (inTestMode) {
      static long lastCount = 0;
      long count = ms / 200;
      if (count > lastCount) {
        lastCount = count;
        digitalWrite(ledPinBANK, lastCount % 2);
      }
    }
  }
  return false;
}

void handleCV(CVModData& data) {
  int val = 0;
  if (state_progCVProgramEverywhere) {
    for (int i = 0; i < data.bank; i++) {
      val += bankSizes[i];
    }
    val += data.prog;
    val += round(data.cvnorm * float(totalPrograms * 2 + 1));
    val %= totalPrograms;
    while (val < 0) val += totalPrograms;

    int newBank = 0;
    int newProg = 0;
    bool foundIt = false;
    for (int i = 0; i < numBanks; i++) {
      if (val - bankSizes[i] < 0) { // in this bank
        newBank = i;
        newProg = val;
        foundIt = true;
        break;
      }
      else {
        val -= bankSizes[i];
      }
    }
    if (!foundIt) {
      Serial.println("DID NOT FIND NEW PROGRAM");
    }
    else {
      data.selection.setBank(newBank);
      data.selection.setProgram(newProg);
    }
  }
  else {
    val = data.prog;
    val += round(data.cvnorm * float(programsPerBank * 2));
    val %= data.numProgramsInCurrentBank;
    while (val < 0) val += data.numProgramsInCurrentBank;
    data.selection.setProgram(val);
  }
}

void loopDisplay() {
  sevseg.refreshDisplay();
}
