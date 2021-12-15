// Noise Plethora
// Copyright (c) 2021 Befaco / Jeremy Bernstein
// Open-source software
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported
// See LICENSE.txt for the complete license text

// white noise on pin 14
#define speakerPin 14
unsigned long lastClick;
/* initialize with any 32 bit non-zero  unsigned long value. */
#define LFSR_INIT  0xfeedfaceUL
/* Choose bits 32, 30, 26, 24 from  http://arduino.stackexchange.com/a/6725/6628
 *  or 32, 22, 2, 1 from
 *  http://www.xilinx.com/support/documentation/application_notes/xapp052.pdf
 *  or bits 32, 16, 3,2  or 0x80010006UL per http://users.ece.cmu.edu/~koopman/lfsr/index.html
 *  and http://users.ece.cmu.edu/~koopman/lfsr/32.dat.gz
 */
#define LFSR_MASK  ((unsigned long)( 1UL<<31 | 1UL <<15 | 1UL <<2 | 1UL <<1  ))

void setupNoise() {
  pinMode(speakerPin, OUTPUT);
  lastClick = micros();
}

unsigned int generateNoise() {
  // See https://en.wikipedia.org/wiki/Linear_feedback_shift_register#Galois_LFSRs
  static unsigned long int lfsr = LFSR_INIT;  /* 32 bit init, nonzero */
  /* If the output bit is 1, apply toggle mask.
   * The value has 1 at bits corresponding
   * to taps, 0 elsewhere.
   */

  if (lfsr & 1) {
    lfsr = (lfsr >> 1) ^ LFSR_MASK;
    return(1);
  }
  else {
    lfsr >>= 1;
    return(0);
  }
}

void loopNoise() {
  if ((micros() - lastClick) > 5) { // Changing this value changes the frequency.
    lastClick = micros();
    digitalWrite(speakerPin, generateNoise());
  }
}
