#include <Streaming.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <LiquidCrystal.h>
#include <math.h>

#include "Display.h"
#include "Steppers.h"
#include "Menu.h"

Steppers theSteppers;

void setup() {
   Serial.begin(57600);
   
   theDisplay.Setup();
   theSteppers.Setup();
   theUiMaster.Setup();

   // theDisplay << _BLINK(true) << _CURSOR(0,0) << 128 << endl << _BLINK(false) << _FLOAT(123.4245f, 3);
   // theSteppers.MoveTo({0.0f, 1.0f});
   // theSteppers.Turn(M_PI * 2.0f);
   theSteppers.Move(300.0f * 3.0f);
}

void loop() {
   theSteppers.Poll();
   theDisplay.Poll();
   //static int x = 0;
   //x++;
   //theDisplay << _BLINK(true) << _CURSOR(0,0) << x << endl << _BLINK(false) << _FLOAT(123.4245f, 3);
   theUiMaster.Poll();
}
