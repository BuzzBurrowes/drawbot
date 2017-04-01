/*****************************************************************************
   StepperSet
   ==========
   This is a templated class that can control synchronize multiple steppers. 
   This allows for multiple steppers that can accelerate smoothly yet stay
   in "lock step".

   by Buzz Burrowes
*****************************************************************************/
#pragma once

#include "AcceleratingStepperController.h"

// STEPPERTYPE must be a class derived from AcceleratingStepperController.
template <typename STEPPERTYPE, int NUMSTEPPERS>
class StepperSet {
public:
   StepperSet(STEPPERTYPE* steppers) {
      for (int i = 0; i < NUMSTEPPERS; ++i)
         mSteppers[i] = steppers  + i;
   }

//#define TO_FLOAT(x) ((float)(x) / (float)(1L << 8))
   void Poll() {  
      uint32_t currentMics = micros();
      
      // poll the master...
      mSteppers[mMasterIndex]->Poll(currentMics);
      SignedFixedPoint<8> fractionMasterStep = mSteppers[mMasterIndex]->GetFractionalStepInMove();
      bool masterAtTarget = mSteppers[mMasterIndex]->AtTarget();

      // now deal with the slaves...
      for (int i = 0; i < NUMSTEPPERS; ++i) {
         if (i != mMasterIndex) {
            if (masterAtTarget) {
               mSteppers[i]->MakeNextStepInMove();
               continue;
            }
            int32_t matchingStep = (mStepsPerMaster[i] == 1) ?  fractionMasterStep.Truncated() : (fractionMasterStep * mStepsPerMaster[i]).RoundToInt();
            while (matchingStep != mSteppers[i]->GetStepInMove()) {
               mSteppers[i]->MakeNextStepInMove();
            }
         }
      }
   }
   
   bool Idle() {
      for (int i = 0; i < NUMSTEPPERS; ++i)
      if (!mSteppers[i]->AtTarget()) 
         return false;
      return  true; 
   }

   void AwaitIdle() {
      while (!Idle())
         Poll();
   }

   void Rest() {
      for (int i = 0; i < NUMSTEPPERS; ++i) {
         mSteppers[i]->Rest();
      }
   }

   void SetTargets(int32_t* targets) {
      mMasterIndex = 0;
      for (int i = 0; i < NUMSTEPPERS; ++i) {
         mStepsPerMaster[i] = mSteppers[i]->SetTarget(targets[i]);
         if (i > 0 && mStepsPerMaster[i] > mStepsPerMaster[mMasterIndex])
            mMasterIndex = i;
      }
      SignedFixedPoint<8> masterStepCount = mStepsPerMaster[mMasterIndex];
      for (int i = 0; i < NUMSTEPPERS; ++i) {
         if (mStepsPerMaster[i] == masterStepCount)
            mStepsPerMaster[i] = 1;
         else
            mStepsPerMaster[i] /= masterStepCount;
      }
   }

private:
   STEPPERTYPE* mSteppers[NUMSTEPPERS];
   SignedFixedPoint<8> mStepsPerMaster[NUMSTEPPERS];
   uint8_t      mMasterIndex {0xFF};
};