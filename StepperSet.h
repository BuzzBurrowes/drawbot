#pragma once

#include "Stepper.h"

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
      uint32_t fractionMasterStep = mSteppers[mMasterIndex]->GetFractionalStepInMove();
      bool masterAtTarget = mSteppers[mMasterIndex]->AtTarget();

      // now deal with the slaves...
      for (int i = 0; i < NUMSTEPPERS; ++i) {
         if (i != mMasterIndex) {
            if (masterAtTarget) {
               mSteppers[i]->MakeNextStepInMove();
               continue;
            }
            int32_t matchingStep = (((int32_t)fractionMasterStep * mStepsPerMaster[i])+0x80) >> 16;
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

   void SetTargets(int32_t* targets) {
      mMasterIndex = 0;
      for (int i = 0; i < NUMSTEPPERS; ++i) {
         mStepsPerMaster[i] = mSteppers[i]->SetTarget(targets[i]);
         if (i > 0 && mStepsPerMaster[i] > mStepsPerMaster[mMasterIndex])
            mMasterIndex = i;
      }
      int32_t masterStepCount = mStepsPerMaster[mMasterIndex];
      for (int i = 0; i < NUMSTEPPERS; ++i) {
         mStepsPerMaster[i] = (mStepsPerMaster[i]<<8) / masterStepCount;
      }
   }

private:
   STEPPERTYPE* mSteppers[NUMSTEPPERS];
   int32_t      mStepsPerMaster[NUMSTEPPERS];
   uint8_t      mMasterIndex {0xFF};
};