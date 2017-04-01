#include <Arduino.h>
#include <Streaming.h>
#include <math.h>

#include "Steppers.h"

//static const float kWheelDiameterMm      = 55.5f;
//static const float kWheelCircumferenceMm = (kWheelDiameterMm * M_PI);
//static const float kWheelCircumferenceMm = 291.34f; // 15 point star;
static const float kWheelCircumferenceMm = 291.2f; // 15 point star;
static const float kCartWidthMm          = 160.3375f;
static const float kStepsPerRotation     = 2047.3f;

Steppers::Steppers()
   : mStepper{{S0_0, S0_1, S0_2, S0_3},
              {S1_0, S1_1, S1_2, S1_3}}
   , mController{mStepper}
   , mHeading{1.0f, 0.0f} {
   for (int i = 0; i < kNumSteppers; ++i) { 
      mStepper[i].SetBalistics(500,500);
   }
}

void Steppers::SetZero() {
   mPosition = {0.f,0.f};
   mHeading = {1.f,0.f};
}

void Steppers::Poll() {
   mController.Poll();
}

void Steppers::TestStepCount() {
   static const int stepsPerRev = 4096;
   Serial.println("Running test...");
   int32_t targets[] {2048U * 5, (int32_t)-2048 * 5};
   mController.SetTargets(targets);
   mController.AwaitIdle();
   return;
   targets[0] = 0;
   targets[1] = 0;
   mController.SetTargets(targets);
   mController.AwaitIdle();
   Serial.println("TEST DONE!");
}

bool Steppers::Idle() {
   return mController.Idle();
}

void Steppers::AwaitArrival() {
   mController.AwaitIdle();
}

void Steppers::Rest() {
   mController.Rest();
}

void Steppers::MoveTo(const Vec2dF& pos) {
   Vec2dF relative = pos - mPosition;
   float dir = 0;
   float angle = mHeading.RadiansTo(relative, &dir);
   Serial << "angle: " << angle << ", dir: " << dir << endl; 
   if (angle != 0.0f) {
      angle *= (dir < 0.0f) ? -1 : 1;
      Turn(angle);
   }
   float distanceCm = relative.Magnitude();
   Serial << "distance: " << distanceCm << endl;
   Move(distanceCm);
}

void Steppers::Move(float distanceMm) {
   float rotations        = distanceMm / kWheelCircumferenceMm;
   float steps            = rotations * kStepsPerRotation;
   int   actualSteps      = (int)round(steps);
   float actualDistanceMm = ((float)actualSteps / kStepsPerRotation) * kWheelCircumferenceMm;
   mPosition += mHeading * actualDistanceMm;
   _MoveRelative(actualSteps, actualSteps);   
}

void Steppers::Turn(float radians) {
   float fullCircumferenceMm = kCartWidthMm * M_PI;
   float wheelDistanceMm     = fullCircumferenceMm * radians / (M_PI * 2.0f);
   float rotations           = wheelDistanceMm / kWheelCircumferenceMm;
   float steps               = rotations * kStepsPerRotation;
   int   actualSteps         = (int)round(steps);
   float actualDistanceMm    = ((float)actualSteps / kStepsPerRotation) * kWheelCircumferenceMm;
   float actualRadians       = (actualDistanceMm * (M_PI * 2.0f)) / fullCircumferenceMm;
   mHeading.Rotate(actualRadians);
   mHeading.Normalize();
   _MoveRelative(actualSteps, -actualSteps);   
}

void Steppers::TurnDegrees(float degrees) {
   Turn((M_PI * degrees) / 180.f);
}

void Steppers::_MoveRelative(long stepsL, long stepsR) {
   // make sure last move is finished...
   mController.AwaitIdle();

   Serial << "_MoveRelative: " << stepsL << ", " << stepsR << endl;
   long positions[2];
   positions[0] = mStepper[0].CurrentPosition() + stepsL;
   positions[1] = mStepper[1].CurrentPosition() + stepsR;
   mController.SetTargets(positions);
}
