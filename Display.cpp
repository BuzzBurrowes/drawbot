#include <Arduino.h>
#include <Streaming.h>

#include "Display.h"

Display  theDisplay;

Display::Display()
   : mLcd(kPinRS, kPinEnable, kPinD0, kPinD1, kPinD2, kPinD3)
   , mBufferDirty(true)
   , mCurrentDisplay(0)
   , mNextFlipMs(0)
   , mCursorX(0)
   , mCursorY(0)
   , mBlink(false) {
   memset(mDisplayBuffer, ' ', kDisplayBufferSize);
   mDisplayBuffer[0][0][kColumns] = 0;
   mDisplayBuffer[0][1][kColumns] = 0;
   mDisplayBuffer[1][0][kColumns] = 0;
   mDisplayBuffer[1][1][kColumns] = 0;
}

void Display::Setup() {
   mLcd.begin(kColumns, kRows);
}

void Display::Poll() {
   bool needsFlip = millis() > mNextFlipMs;
   if (needsFlip || mBufferDirty) _Refresh(needsFlip);
}

size_t Display::write(uint8_t byte) {
   if (byte == '\r' || byte == '\t') return;
   if (byte == '\n')
   {
      mCursorX = 0;
      mCursorY++;
      mCursorY %= kRows;
      return;
   }
   mDisplayBuffer[0][mCursorY][mCursorX] = (char)byte;
   if (!mBlink)
      mDisplayBuffer[1][mCursorY][mCursorX] = (char)byte;
   mCursorX++;
   mCursorX %= kColumns;
   mBufferDirty = true;
}

void Display::_Refresh(bool flip) {
   if (flip) {
      mCurrentDisplay = !mCurrentDisplay;
      mNextFlipMs = millis() + kBlinkRateMs;
   }
   mLcd.setCursor(0,0);
   mLcd.print(mDisplayBuffer[mCurrentDisplay][0]);
   mLcd.setCursor(0,1);
   mLcd.print(mDisplayBuffer[mCurrentDisplay][1]);
   mBufferDirty = false;
}