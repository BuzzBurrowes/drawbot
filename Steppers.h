#pragma once
#include <AccelStepper.h>
#include <MultiStepper.h>
#include "Vec2d.h"

class Steppers {
public:
   Steppers();
   void Setup() {}
   void Poll();
   bool Idle();

   static const int kNumSteppers = 2;
   static const int kMaxSpeed    = 500;
   static const uint8_t S0_0     = 10;
   static const uint8_t S0_1     = 11;
   static const uint8_t S0_2     = 12;
   static const uint8_t S0_3     = 13;
   static const uint8_t S1_0     = A0;
   static const uint8_t S1_1     = A1;
   static const uint8_t S1_2     = A2;
   static const uint8_t S1_3     = A3;

   void MoveTo(const Vec2dF& pos);
   void Move(float distanceCm);
   void Turn(float radians);
   const Vec2dF Position() const { return mPosition; }
   const Vec2dF Heading() const  { return mHeading;  }

private:
   AccelStepper mStepper[2];
   MultiStepper mController;

   Vec2dF mPosition;
   Vec2dF mHeading;

   void _MoveRelative(long stepsL, long stepsR);
};
