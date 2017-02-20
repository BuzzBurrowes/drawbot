#include <Arduino.h>

#include "Menu.h"

UiMaster theUiMaster;

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
UiMenu mainMenu("Main");

UiMaster::Setup() {
   dynamicsMenu.Add(speedElemnt);
   dynamicsMenu.Add(accelElement);
   thresholdsMenu.Add(minSetpointElemnt);
   thresholdsMenu.Add(maxSetpointElement);
   mainMenu.Add(dynamicsMenu);
   mainMenu.Add(thresholdsMenu);   
}
