#include <Arduino.h>

#include "Menu.h"
#include "Display.h"

UiMaster theUiMaster;

class AppState {
public:
  float maxSpeed = 100.0f;
  float maxAccel = 3.45f;
  float minSetpoint = 2378.95f;
  float maxSetpoint = 1238764.293f;
};

AppState theState;

UiMenu dynamicsMenu("Dynamics");
UiElement speedElemnt("Max Speed", 
          [](){ theUiMaster.DrawContent(theState.maxSpeed); },
          [](){ theUiMaster.FloatEditor(theState.maxSpeed); });
UiElement accelElement("Max Acceleration", 
          [](){ theUiMaster.DrawContent(theState.maxAccel); },
          [](){ theUiMaster.FloatEditor(theState.maxAccel); });

UiMenu thresholdsMenu("Thresholds");
UiElement minSetpointElemnt("Min Setpoint", 
          [](){ theUiMaster.DrawContent(theState.minSetpoint); },
          [](){ theUiMaster.FloatEditor(theState.minSetpoint); });
UiElement maxSetpointElement("Max Setpoint", 
          [](){ theUiMaster.DrawContent(theState.maxSetpoint); },
          [](){ theUiMaster.FloatEditor(theState.maxSetpoint); });
UiMenu mainMenu("Main Menu");

UiMaster::UiMaster() {
  
}

void UiMaster::Setup() {
   dynamicsMenu.Add(speedElemnt);
   dynamicsMenu.Add(accelElement);
   thresholdsMenu.Add(minSetpointElemnt);
   thresholdsMenu.Add(maxSetpointElement);
   mainMenu.Add(dynamicsMenu);
   mainMenu.Add(thresholdsMenu);
   mCurrentUiElement = &mainMenu;   
}

void UiMaster::Poll() {
  static int nextRefreshMs = 0;
  if (nextRefreshMs < millis()) {
    if (mCurrentUiElement) {
      theDisplay << _CURSOR(0,0) << mCurrentUiElement->GetName();
      mCurrentUiElement->Poll();
    }
    nextRefreshMs = millis() + 30;    
  }
}

void UiMaster::DrawContent(float& content) {
  //theDisplay << content;
}

void UiMaster::FloatEditor(float& content) {
  theDisplay << "Hello World!";
  return;
}

void UiMaster::SetCurrentUiElement(UiElement* el) {
  mCurrentUiElement = el;
}

UiElement::UiElement(const char* name) 
  : mName(name) 
  , mDrawContentFunc(nullptr)
  , mPollFunc(nullptr) {

}

UiElement::UiElement(const char* name, UiCallbackFunc drawContentFunc,
      UiCallbackFunc pollFunc) 
  : mName(name)
  , mDrawContentFunc(drawContentFunc)
  , mPollFunc(pollFunc) {

}

void UiMenu::Add(UiElement& element) {
  if (mFirstChild == nullptr) {
    mFirstChild  = &element;
    mCurrentItem = &element;
    return;
  }
  UiElement* walker = mFirstChild;
  while (walker->GetNext() != nullptr)
    walker = walker->GetNext();
  walker->SetNext(&element);
}

