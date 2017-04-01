/*****************************************************************************
   AcceleratingStepperController
   =============================
   This is a pure virtual, base class for a "stepper" controller. It handles 
   the smooth acceleration of a stepper to move from position to position.

   The balistics are based on this article:
   http://www.embedded.com/design/mcus-processors-and-socs/4006438/Generate-stepper-motor-speed-profiles-in-real-time

   by Buzz Burrowes
*****************************************************************************/
#pragma once

#include "fixedpoint.h"

class AcceleratingStepperController {
public:
   static const int kMantissa = 8;
   virtual ~AcceleratingStepperController() {}

   void SetBalistics(int32_t maxStepsPerSec, int accelStepsPerSecPerSec);

   void SetHome() { mCurrentPos = mTargetPos = 0; }
   bool AtHome()  { return (mCurrentPos == mTargetPos) && mCurrentPos == 0; }
   
   // This returns the number of steps needed to reach the target.
   int32_t SetTarget(int32_t t);
   bool    AtTarget() { return mCurrentPos == mTargetPos; }

   virtual void Step(int8_t dir) = 0;
   virtual void Rest() = 0;

   int32_t CurrentPosition() { return mCurrentPos; }
   int32_t TargetPosition()  { return mTargetPos;  }

   SignedFixedPoint<kMantissa> GetFractionalStepInMove() { return mFractionalStep; }
   uint32_t GetStepInMove() { return (uint32_t)mNextStep; }

   // These return true if a step was made during the poll.
   bool Poll() { return Poll(micros()); }
   bool Poll(uint32_t newMics);

   void MakeNextStepInMove();

private:
   enum class RampState {
      kRampIdle,
      kRampUp,  
      kRampMax, 
      kRampDown
   };

   uint32_t mMinMicrosecondsPerStep {1100};
   uint32_t mInitialStep            {44000};
   int32_t  mCurrentPos             {0};
   int32_t  mTargetPos              {0};
   int32_t  mStepsInCurrentMove     {0};
   int32_t  mMoveMidpoint           {0};
   SignedFixedPoint<kMantissa> mHiResMicsUntilNextStep {0};
   int32_t  mMicsUntilNextStep      {0};
   int32_t  mDir                    {1};
   int32_t  mNextStep               {0};
   int32_t  mDenom                  {1}; // 4.n+1 in ramp algorithm
   int32_t  mDecelStart             {0}; 
   SignedFixedPoint<kMantissa> mFractionalStep {0};
   uint32_t  mLastMics              {0};
   
   RampState mRampState {RampState::kRampIdle};
   bool      mRunning   {false};

   bool _CheckStep(uint32_t deltaMics);
};
