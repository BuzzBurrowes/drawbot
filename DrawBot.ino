#include <Streaming.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <LiquidCrystal.h>

#include "Display.h"
#include "Steppers.h"

Display  theDisplay;
Steppers theSteppers;

void setup() {
   Serial.begin(57600);
   
   theDisplay.Setup();
   theSteppers.Setup();

   theDisplay << _BLINK(true) << _CURSOR(0,0) << 128 << endl << _BLINK(false) << _FLOAT(123.4245f, 3);
   theSteppers.MoveTo({0.0f, 1.0f});
}

void loop() {
  theSteppers.Poll();
  theDisplay.Poll();
}
