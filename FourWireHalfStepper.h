/*****************************************************************************
   FourWireHalfStepper
   ===================
   An implementation of AcceleratingStepperController for controlling a four
   wire stepper with an eight step sequence. 

   by Buzz Burrowes
*****************************************************************************/
#pragma once

#include "AcceleratingStepperController.h"

class FourWireHalfStepper : public AcceleratingStepperController {
public:
   FourWireHalfStepper(uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3) 
      : mPin {pin0, pin1, pin2, pin3} {
      pinMode(pin0, OUTPUT);
      pinMode(pin1, OUTPUT);
      pinMode(pin2, OUTPUT);
      pinMode(pin3, OUTPUT);
   }

   virtual void Step(int8_t dir) override {
      mCurrentPhase += dir;
      mCurrentPhase %= kNumPhases;
      for (int i = 0; i < 4; ++i) {
         if (!(mPhasePins[mCurrentPhase] & (1<<i)))
            digitalWrite(mPin[i], LOW);
      }
      for (int i = 0; i < 4; ++i) {
         if (mPhasePins[mCurrentPhase] & (1<<i)) {
            digitalWrite(mPin[i], HIGH);
         }
      }
   }

   virtual void Rest() override {
      for (int i = 0; i < 4; ++i) {
         digitalWrite(mPin[i], LOW);
      }
   }

private:
   static const int kNumPhases = 8;
   uint8_t mPin[kNumPhases] {0, 0, 0, 0};
   uint8_t mPhasePins[kNumPhases] {0b1000, 0b1100, 0b0100, 0b0110, 0b0010, 0b0011, 0b0001, 0b1001};
   uint8_t mCurrentPhase {0};
};

