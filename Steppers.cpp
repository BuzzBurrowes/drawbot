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
#if USE_ACCELSTEPPER
   : mStepper{{AccelStepper::HALF4WIRE, S0_0, S0_2, S0_1, S0_3},
              {AccelStepper::HALF4WIRE, S1_0, S1_2, S1_1, S1_3}}
#else
   : mStepper{{S0_0, S0_1, S0_2, S0_3},
              {S1_0, S1_1, S1_2, S1_3}}
   , mController{mStepper}
#endif 
   , mHeading{1.0f, 0.0f} {
   for (int i = 0; i < kNumSteppers; ++i) { 
#if USE_ACCELSTEPPER
      mStepper[i].setMaxSpeed(kMaxSpeed);
      mController.addStepper(mStepper[i]);
#else
      mStepper[i].SetBalistics(500,500);
#endif
   }
}

void Steppers::SetZero() {
   mPosition = {0.f,0.f};
   mHeading = {1.f,0.f};
}

void Steppers::Poll() {
#if USE_ACCELSTEPPER
   mController.run();
#else
   mController.Poll();
#endif
}

void Steppers::TestStepCount() {
   static const int stepsPerRev = 4096;
   Serial.println("Running test...");

#if USE_ACCELSTEPPER

   mStepper[1].setAcceleration(kMaxSpeed);
   mStepper[1].setSpeed(kMaxSpeed);
   mStepper[1].setCurrentPosition(0);
   mStepper[1].moveTo(stepsPerRev * 10);
   mStepper[1].runSpeedToPosition();
   //mStepper[1].runToNewPosition(stepsPerRev * 10);
#else 
#if 0
   Serial << "Fixed Point Test\n";
   SignedFixedPoint<8> fixed1;
   fixed1 = 15.5f;
   Serial << _HEX((int32_t)fixed1) << endl;
   SignedFixedPoint<8> test(3);
   Serial << _HEX((int32_t)test) << endl;
   test+=fixed1;
   Serial << _HEX((int32_t)test) << endl;
   test*=fixed1;
   Serial << _HEX((int32_t)test) << endl;
   ++test;
   Serial << _HEX((int32_t)test) << endl;
   test/=fixed1;
   Serial << _HEX((int32_t)test) << endl;
   test*=2;
   Serial << _HEX((int32_t)test) << endl;
   test/=2;
   Serial << _HEX((int32_t)test) << endl;
   test+=4;
   Serial << _HEX((int32_t)test) << endl;
   test-=5;
   Serial << _HEX((int32_t)test) << endl;
   while(1);
#endif
   int32_t targets[] {2048U * 5, (int32_t)-2048 * 5};
   mController.SetTargets(targets);
   mController.AwaitIdle();
   return;
   targets[0] = 0;
   targets[1] = 0;
   mController.SetTargets(targets);
   mController.AwaitIdle();
#endif
   Serial.println("TEST DONE!");
}

bool Steppers::Idle() {
#if USE_ACCELSTEPPER
   return !mStepper[0].isRunning() && !mStepper[1].isRunning();
#else
   return mController.Idle();
#endif
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
#if USE_ACCELSTEPPER
   if (!Idle()) mController.runSpeedToPosition();
#else
   mController.AwaitIdle();
#endif

   Serial << "_MoveRelative: " << stepsL << ", " << stepsR << endl;
   long positions[2];
#if USE_ACCELSTEPPER
   positions[0] = mStepper[0].currentPosition() + stepsL;
   positions[1] = mStepper[1].currentPosition() + stepsR;
   mController.moveTo(positions);
#else   
   positions[0] = mStepper[0].CurrentPosition() + stepsL;
   positions[1] = mStepper[1].CurrentPosition() + stepsR;
   mController.SetTargets(positions);
#endif
}
