#include <Streaming.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include <math.h>

#include "Display.h"
#include "Steppers.h"
#include "Menu.h"

Steppers theSteppers;
Servo thePenServo;

static unsigned long gLastMilli = 0;

void PenUp() {
   thePenServo.write(20);
}

void PenDown() {
   thePenServo.write(160);
}

void setup() {
   Serial.begin(115200);
   
   theDisplay.Setup();
   theSteppers.Setup();
   thePenServo.attach(9);
   theUiMaster.Setup();

   // theDisplay << _BLINK(true) << _CURSOR(0,0) << 128 << endl << _BLINK(false) << _FLOAT(123.4245f, 3);
   // theSteppers.MoveTo({0.0f, 1.0f});
   // theSteppers.Turn(M_PI * 2.0f);
   //theSteppers.Move(300.0f * 3.0f);
   gLastMilli = millis();
}

void loop() {
   theSteppers.TestStepCount();
   while(1);
   unsigned long lastMilli = gLastMilli;
   gLastMilli = millis();
   unsigned long deltaMillis = (lastMilli < gLastMilli) ? gLastMilli - lastMilli : (0xFFFFFFFF - lastMilli) + gLastMilli; 
 
   if ((gLastMilli / 5000)%2)
      PenUp();
   else 
      PenDown();

   theSteppers.Poll();
   theDisplay.Poll();
   //static int x = 0;
   //x++;
   //theDisplay << _BLINK(true) << _CURSOR(0,0) << x << endl << _BLINK(false) << _FLOAT(123.4245f, 3);
   theUiMaster.Poll();
}
