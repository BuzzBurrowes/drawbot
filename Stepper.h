#ifndef _STEPPER_H
#define _STEPPER_H

#include "fixedpoint.h"


template <typename S>
class StepperController : public S {
public:
   using S::S;
   static const int gMantissa = 8;

   void SetBalistics(int32_t maxStepsPerSec, int accelStepsPerSecPerSec) {
      mMinMicrosecondsPerStep = 1000000L / maxStepsPerSec;
      mInitialStep = (int32_t)(1000000.0f * sqrt(2.0f/(float)accelStepsPerSecPerSec));
   }   

   void SetHome() {
      mCurrentPos = mTargetPos = 0;
   }

   bool AtHome() {
      return (mCurrentPos == mTargetPos) && mCurrentPos == 0;
   }

   int32_t SetTarget(int32_t t) {
      if (t != mCurrentPos)
         return _MoveToTarget(t);
      else 
         return 0;
   }

   void Rest() {
     _Rest(); 
   }

   bool AtTarget() {
      return mCurrentPos == mTargetPos;
   }

   int32_t CurrentPosition() { return mCurrentPos; }
   int32_t TargetPosition()  { return mTargetPos;  }

   SignedFixedPoint<gMantissa> GetFractionalStepInMove() { return mFractionalStep; }
   uint32_t GetStepInMove() { return (uint32_t)mNextStep; }

   bool Poll() {
      return Poll(micros());
   }

   bool Poll(uint32_t newMics) {
      // update clock...
      uint32_t deltaMics = newMics - mLastMics;
      mLastMics = newMics;
      
      if (!AtTarget())
         return _CheckStep(deltaMics);
      else 
         return false;
   }

   void MakeNextStepInMove() {
      if (mCurrentPos == mTargetPos) return;
      Step(mDir);
      mNextStep++;
      mCurrentPos += mDir;
      mFractionalStep = mNextStep;
   }

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

   int32_t  mStepsInCurrentMove     = 0;
   int32_t  mMoveMidpoint           = 0;
   SignedFixedPoint<gMantissa> mHiResMicsUntilNextStep = 0;
   int32_t  mMicsUntilNextStep      = 0;
   int32_t  mDir                    = 1;
   int32_t  mNextStep               = 0;
   int32_t  mDenom                  = 1; // 4.n+1 in ramp algo
   int32_t  mDecelStart             = 0; // start of decel
   SignedFixedPoint<gMantissa> mFractionalStep = 0;
   uint32_t  mLastMics              = 0;

   RampState mRampState = RampState::kRampIdle;
   bool      mRunning   = false;

   bool _CheckStep(uint32_t deltaMics)
   {
      if (mCurrentPos == mTargetPos) return false;

      // early out if we aren't ready for a step...
      if (mMicsUntilNextStep > (int32_t)(deltaMics >> 1)) {
         mMicsUntilNextStep -= deltaMics;
         mFractionalStep = mNextStep;
         //Serial << "frac 1 " << mFractionalStep;
         if(mMicsUntilNextStep < 0) {
            mFractionalStep += SignedFixedPoint<gMantissa>::FromRaw((1 << gMantissa) - 1);
            //Serial << " frac 2 " << mFractionalStep;
         }
         else {
            //Serial << " hr " << mHiResMicsUntilNextStep << " "; 
            SignedFixedPoint<gMantissa> t1 = (mHiResMicsUntilNextStep - mMicsUntilNextStep);
            //Serial << " t1 " << t1; 
            mFractionalStep += t1 / mHiResMicsUntilNextStep;
            //Serial << " frac 3 " << mFractionalStep;
         }
         //Serial << " done" << endl;
         return false;
      }

      Step(mDir);
      ++mNextStep;
      mCurrentPos += mDir;
      mFractionalStep = mNextStep;

      if (mCurrentPos == mTargetPos) {
         mRampState = RampState::kRampIdle;
         //Serial << "Stepper State Idle\n";
         return true;
      }

      switch (mRampState)
      {
      case RampState::kRampUp:
         // first of all, should we switch to deceleration?
         if (mNextStep == mMoveMidpoint) {
            mRampState = RampState::kRampDown;
            //Serial << "Stepper State RampDown\n";
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
         //Serial << "Stepper State RampDown (2)\n";
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
            //Serial << "Stepper State RampMax\n";
            mDecelStart = mStepsInCurrentMove - mNextStep;
            //Serial << "Decel Start = " << mDecelStart << endl;
            mMicsUntilNextStep = mMinMicrosecondsPerStep;
            //Serial << "Mics Until next = " << mMicsUntilNextStep << endl;
            mHiResMicsUntilNextStep = mMicsUntilNextStep; 
            mDenom = ((mDecelStart - mStepsInCurrentMove)<<2)+1;
         }
      }
      else {
         mMicsUntilNextStep = mHiResMicsUntilNextStep.RoundToInt(); // - mMicsUntilNextStep; // round 24.8format->int16
      }

      return true;
   }  

   int32_t _MoveToTarget(int32_t t)
   {
      mTargetPos = t;
      if (t < mCurrentPos) {
         mStepsInCurrentMove = mCurrentPos - t;
         mDir = -1;
      }
      else {
         mStepsInCurrentMove = t - mCurrentPos;
         mDir = 1;
      }
      mMoveMidpoint = mStepsInCurrentMove / 2;

      mMicsUntilNextStep      = mInitialStep;
      mHiResMicsUntilNextStep = mMicsUntilNextStep; 
      //Serial << "Hi Res: " << mHiResMicsUntilNextStep << endl;
      mNextStep = 0;  
      mDecelStart = 0;
      mDenom = 1;     
      mRampState = RampState::kRampUp; 
      mRunning = true;
      mFractionalStep = (int32_t)0;
      return mStepsInCurrentMove;
   }
};

class FourWireHalfSteppable {
public:
   FourWireHalfSteppable(uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3) 
      : mPin {pin0, pin1, pin2, pin3}
   {}

   void Step(int8_t dir) {
      mCurrentPhase += dir;
      mCurrentPhase %= kNumPhases;
      for (int i = 0; i < 4; ++i)
      {
         if (!(mPhasePins[mCurrentPhase] & (1<<i)))
            digitalWrite(mPin[i], LOW);
      }
      for (int i = 0; i < 4; ++i)
      {
         if (mPhasePins[mCurrentPhase] & (1<<i)) {
            digitalWrite(mPin[i], HIGH);
         }
      }
   }

   void _Rest() {
      for (int i = 0; i < 4; ++i) {
         digitalWrite(mPin[i], LOW);
      }
   }

private:
   static const int kNumPhases = 8;
   uint8_t mPin[4]       = {0, 0, 0, 0};
   uint8_t mPhasePins[kNumPhases] = {0b1000, 0b1100, 0b0100, 0b0110, 0b0010, 0b0011, 0b0001, 0b1001 };
   uint8_t mCurrentPhase = {0};
};

class FourWireFullSteppable {
public:
   FourWireFullSteppable(uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3) 
      : mPin {pin0, pin1, pin2, pin3} {
      pinMode(pin0, OUTPUT);
      pinMode(pin1, OUTPUT);
      pinMode(pin2, OUTPUT);
      pinMode(pin3, OUTPUT);
   }

   void Step(int8_t dir) {
      mCurrentPhase += dir;
      mCurrentPhase %= kNumPhases;
      for (int i = 0; i < 4; ++i)
      {
         if (!(mPhasePins[mCurrentPhase] & (1<<i)))
            digitalWrite(mPin[i], LOW);
      }
      for (int i = 0; i < 4; ++i)
      {
         if (mPhasePins[mCurrentPhase] & (1<<i)) {
            digitalWrite(mPin[i], HIGH);
         }
      }
   }

   void _Rest() {
      for (int i = 0; i < 4; ++i) {
         digitalWrite(mPin[i], LOW);
      }
   }

private:
   static const int kNumPhases = 4;
   uint8_t mPin[4]       = {0, 0, 0, 0};
   uint8_t mPhasePins[kNumPhases] = {0b1100, 0b0110, 0b0011, 0b1001 };
   uint8_t mCurrentPhase = {0};
};

using FourWireHalfStepper = StepperController<FourWireHalfSteppable>;
using FourWireFullStepper = StepperController<FourWireFullSteppable>;

#endif // _STEPPER_H
