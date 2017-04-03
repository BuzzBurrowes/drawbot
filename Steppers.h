#pragma once
#define  USE_ACCELSTEPPER 0
#if USE_ACCELSTEPPER
#include <AccelStepper.h>
#include <MultiStepper.h>
#else
#include "StepperSet.h"
#include "FourWireFullStepper.h"
#endif
#include "Vec2d.h"

class Steppers {
public:
   Steppers();
   void Setup() {}
   void SetZero();
   void TestStepCount();
   void Poll();
   bool Idle();
   void AwaitArrival();
   void Rest();

   static const int kNumSteppers = 2;
   static const int kMaxSpeed    = 35900;
   static const uint8_t S0_0     = 4;
   static const uint8_t S0_1     = 5;
   static const uint8_t S0_2     = 6;
   static const uint8_t S0_3     = 7;
   static const uint8_t S1_0     = A3;
   static const uint8_t S1_1     = A2;
   static const uint8_t S1_2     = A1;
   static const uint8_t S1_3     = A0;

   void MoveTo(const Vec2dF& pos);
   void Move(float distanceCm);
   void Turn(float radians);
   void TurnDegrees(float degrees);
   const Vec2dF Position() const { return mPosition; }
   const Vec2dF Heading() const  { return mHeading;  }

private:
   FourWireFullStepper mStepper[2];
   StepperSet<FourWireFullStepper,2> mController;

   Vec2dF mPosition;
   Vec2dF mHeading;

   void _MoveRelative(long stepsL, long stepsR);
};
