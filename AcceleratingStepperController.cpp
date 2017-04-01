/*****************************************************************************
   AcceleratingStepperController
   =============================
   This is a pure virtual, base class for a "stepper" controller. It handles 
   the smooth acceleration of a stepper to move from position to position.

   The balistics are based on this article:
   http://www.embedded.com/design/mcus-processors-and-socs/4006438/Generate-stepper-motor-speed-profiles-in-real-time

   by Buzz Burrowes
*****************************************************************************/
#include <Arduino.h>
#include "AcceleratingStepperController.h"

void AcceleratingStepperController::SetBalistics(int32_t maxStepsPerSec, int accelStepsPerSecPerSec) {
   mMinMicrosecondsPerStep = 1000000L / maxStepsPerSec;
   mInitialStep = (int32_t)(1000000.0f * sqrt(2.0f/(float)accelStepsPerSecPerSec));
}   

int32_t AcceleratingStepperController::SetTarget(int32_t t) {
   if (t == mCurrentPos) return 0;
   mTargetPos = t;
   if (t < mCurrentPos) {
      mStepsInCurrentMove = mCurrentPos - t;
      mDir = -1;
   }
   else {
      mStepsInCurrentMove = t - mCurrentPos;
      mDir = 1;
   }
   mMoveMidpoint           = mStepsInCurrentMove / 2;
   mMicsUntilNextStep      = mInitialStep;
   mHiResMicsUntilNextStep = mMicsUntilNextStep; 
   mNextStep               = 0;  
   mDecelStart             = 0;
   mDenom                  = 1;     
   mRampState              = RampState::kRampUp; 
   mRunning                = true;
   mFractionalStep         = (int32_t)0;
   
   return mStepsInCurrentMove;
}


bool AcceleratingStepperController::Poll(uint32_t newMics) {
   // update clock...
   uint32_t deltaMics = newMics - mLastMics;
   mLastMics = newMics;
   
   if (!AtTarget())
      return _CheckStep(deltaMics);
   else 
      return false;
}

void AcceleratingStepperController::MakeNextStepInMove() {
   if (mCurrentPos == mTargetPos) return;
   Step(mDir);
   mNextStep++;
   mCurrentPos += mDir;
   mFractionalStep = mNextStep;
}

bool AcceleratingStepperController::_CheckStep(uint32_t deltaMics) {
   if (mCurrentPos == mTargetPos) return false;

   // early out if we aren't ready for a step...
   if (mMicsUntilNextStep > (int32_t)(deltaMics >> 1)) {
      mMicsUntilNextStep -= deltaMics;
      mFractionalStep = mNextStep;
      if(mMicsUntilNextStep < 0) {
         mFractionalStep += SignedFixedPoint<kMantissa>::FromRaw((1 << kMantissa) - 1);
      }
      else {
         SignedFixedPoint<kMantissa> t1 = (mHiResMicsUntilNextStep - mMicsUntilNextStep);
         mFractionalStep += t1 / mHiResMicsUntilNextStep;
      }
      return false;
   }

   Step(mDir);
   ++mNextStep;
   mCurrentPos += mDir;
   mFractionalStep = mNextStep;

   if (mCurrentPos == mTargetPos) {
      mRampState = RampState::kRampIdle;
      return true;
   }

   switch (mRampState)
   {
   case RampState::kRampUp:
      if (mNextStep == mMoveMidpoint) {
         mRampState = RampState::kRampDown;
         mDenom = ((mNextStep - mStepsInCurrentMove) << 2) + 1;
         if (mStepsInCurrentMove & 1) { 
            mDenom += 4;
            break;
         }
      }
      mDenom += 4;
      break;
   case RampState::kRampMax:
      if (mNextStep != mDecelStart)
         break;
      mRampState = RampState::kRampDown;
      break;
   case RampState::kRampDown:
      mDenom += 4;
      break;         
   }

   if (mRampState != RampState::kRampMax) {
      mHiResMicsUntilNextStep -= (mHiResMicsUntilNextStep * (int32_t)2) / mDenom; 
      mMicsUntilNextStep = mHiResMicsUntilNextStep.RoundToInt();  
      if (mMicsUntilNextStep <= mMinMicrosecondsPerStep && mDecelStart == 0) { 
         mRampState = RampState::kRampMax;
         mDecelStart = mStepsInCurrentMove - mNextStep;
         mMicsUntilNextStep = mMinMicrosecondsPerStep;
         mHiResMicsUntilNextStep = mMicsUntilNextStep; 
         mDenom = ((mDecelStart - mStepsInCurrentMove)<<2)+1;
      }
   }
   else {
      mMicsUntilNextStep = mHiResMicsUntilNextStep.RoundToInt(); 
   }
   return true;
}  

