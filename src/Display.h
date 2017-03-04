#pragma once
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <stdint.h>
#include <Streaming.h>

#define _CURSOR(x,y) Display::_Cursor((x),(y))
#define _BLINK(b) Display::_Blink(b)

class Display : public Print {
public:
   Display();
   void Setup();
   void Poll();

   static const int kBlinkRateMs       = 750;
   static const int kColumns           = 16;
   static const int kRows              = 2;
   static const int kPinRS             = 2;
   static const int kPinEnable         = 3;
   static const int kPinD0             = 4;
   static const int kPinD1             = 5;
   static const int kPinD2             = 6;
   static const int kPinD3             = 7;
   static const int kDisplayBufferSize = 2 * kRows * (kColumns + 1);

   void SetCursor(uint8_t x, uint8_t y) { mCursorX = x; mCursorY = y; } 
   void Blink(bool blink) { mBlink = blink; }

   virtual size_t write(uint8_t byte) override;

   struct _Cursor
   {
      uint8_t x;
      uint8_t y;
      explicit _Cursor(uint8_t _x, uint8_t _y) : x(_x), y(_y) {}
   };

   struct _Blink
   {
      bool blink;
      explicit _Blink(bool b) : blink(b) {}
   };

private:
   LiquidCrystal mLcd;
   char          mDisplayBuffer[2][kRows][kColumns+1];
   bool          mBufferDirty;
   bool          mCurrentDisplay;
   uint32_t      mNextFlipMs;
   uint8_t       mCursorX;
   uint8_t       mCursorY;
   bool          mBlink;

   void _Refresh(bool flip);
};

inline Display& operator <<(Display& d, const Display::_Cursor& pos)
{ d.SetCursor(pos.x, pos.y); return d; } 

inline Display& operator <<(Display& d, const Display::_Blink& b)
{ d.Blink(b.blink); return d; } 

template<class T> 
inline Display& operator <<(Display& d, T arg) 
{ d.print(arg); return d; }

inline Display& operator <<(Display& d, const _BYTE_CODE& arg)
{ d.write(arg.val); return d; } 

inline Display& operator <<(Display& d, const _BASED& arg)
{ d.print(arg.val, arg.base); return d; } 

inline Display& operator <<(Display& d, const _FLOAT& arg)
{ d.print(arg.val, arg.digits); return d; }

inline Display& operator <<(Display& d, _EndLineCode arg) 
{ d.println(); return d; }

extern Display theDisplay;