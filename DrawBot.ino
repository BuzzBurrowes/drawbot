#include <Streaming.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include <math.h>

#include "Steppers.h"

#define PEN_SERVO_PIN 2

Steppers theSteppers;
Servo thePenServo;

#define SPIRO      0
#define SQUARE     1
#define DRAW_SHAPE SPIRO

struct Vertex {
   Vec2dF pos;
   bool   visited;

   Vertex() : pos{0.f, 0.f}, visited{false} {}
};

#if DRAW_SHAPE == SPIRO
   static const int   sNumVerts = 9;
   Vertex sVertBuffer[sNumVerts];
   static const float sRadius = 100.f;
   int gStepSize = 0;
#elif DRAW_SHAPE == SQUARE
   static const int   sNumVerts = 5;
   Vertex sVertBuffer[sNumVerts];
   static const float sRadius = 200.f;
   int gStepSize = 1;
#endif

void PenUp() {
   thePenServo.write(110);
}

void PenDown() {
   thePenServo.write(170); 
}

void MakeSpiroVerts(float radius, int numVerts, Vertex* vertBuffer) {
   float radiansBetween = (2.f * M_PI) / (float)numVerts;
   for (int i = 0; i < numVerts; ++i) {
      Vertex& vert = vertBuffer[i];
      float radians = radiansBetween * (float)i;
      float x = radius * cos(radians);
      float y = radius * sin(radians);
      vert.pos = {x, y};
      Serial << "Vert " << i << ": " << x << ", " << y << endl;
   }
}

void ClearVisitedFlags(int numVerts, Vertex* vertBuffer) {
   for (int i = 1; i < numVerts; ++i)
      vertBuffer[i].visited = false;
}

int FindSpiroStep(int numVerts, Vertex* vertBuffer) {
   ClearVisitedFlags(numVerts, vertBuffer);
   int stepSize = (numVerts / 2) - ((numVerts % 2) ? 0 : 1);
   int segments = 0;
   for (;stepSize > 1; --stepSize) {
      //Serial << "Testing step size: " << stepSize << endl; 
      vertBuffer[0].visited = true;
      //Serial << "    visited: 0\n"; 
      int walker = stepSize;
      segments = 1;
      while(!vertBuffer[walker].visited) {
         segments++;
         vertBuffer[walker].visited = true;
         walker += stepSize;
         walker %= numVerts;
      }
      // check if all visited
      int i = 0;
      for (; i < numVerts; ++i)
         if (!vertBuffer[i].visited) break;
         ClearVisitedFlags(numVerts, vertBuffer);
      if (i == numVerts) { //break;
         Serial << "Solution: step = " << stepSize << ", segments = " << segments << endl;
         return stepSize;
      } 
   }
   return 0;
   // if(stepSize == 1)
      //Serial << "No Solution!\n";
}

void setup() {
   Serial.begin(115200);
   
   theSteppers.Setup();
   thePenServo.attach(PEN_SERVO_PIN);

   // theDisplay << _BLINK(true) << _CURSOR(0,0) << 128 << endl << _BLINK(false) << _FLOAT(123.4245f, 3);
#if DRAW_SHAPE == SPIRO  
   MakeSpiroVerts(sRadius, sNumVerts, sVertBuffer);
   gStepSize = FindSpiroStep(sNumVerts, sVertBuffer);
   if (!gStepSize) {
      Serial << "NO SOLUTION!\n";
      while(1);
   }
#elif DRAW_SHAPE == SQUARE
   ClearVisitedFlags(sNumVerts, sVertBuffer);
   sVertBuffer[0].pos = {0.f, 0.f};
   sVertBuffer[1].pos = {sRadius, 0.f};
   sVertBuffer[2].pos = {sRadius, sRadius};
   sVertBuffer[3].pos = {0.f, sRadius};
   sVertBuffer[4].pos = {0.f, 0.f};
   gStepSize = 1;
#endif
   PenUp();
   theSteppers.MoveTo({10.f,0.f});
   theSteppers.AwaitArrival();
   theSteppers.SetZero();
   delay(5000);
}

void loop() {
   static unsigned long sLastMilli = 0;
   unsigned long lastMilli = sLastMilli;
   sLastMilli = millis();
   unsigned long deltaMillis = (lastMilli < sLastMilli) ? sLastMilli - lastMilli : (0xFFFFFFFF - lastMilli) + sLastMilli; 

   static int sCurrentSegment = -1;
   static int sVertIndex = -gStepSize;
   if (theSteppers.Idle()) {
      sCurrentSegment++;
      sVertIndex += gStepSize;
      sVertIndex %= sNumVerts;
      if (sCurrentSegment <= sNumVerts) {
         Serial << "Making segment " << sCurrentSegment << " going to vert " << sVertIndex << endl;
         if (sCurrentSegment > 0)
            PenDown();
         theSteppers.MoveTo(sVertBuffer[sVertIndex].pos);
      }
      else {
         PenUp();
         sCurrentSegment = sNumVerts + 1;
         theSteppers.Rest();
      }
   }

   theSteppers.Poll();
}
