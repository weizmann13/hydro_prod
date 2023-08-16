#ifndef DISPLAYMODULE_H
#define DISPLAYMODULE_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>
#include "TDSModule.h"  // Class for TDS sensor
#include "PhModule.h"   // Class for pH sensor
#include "TempModule.h" // Class for temperature sensor

class DisplayModule
{
public:
    DisplayModule(TDSModule &tdsModule, PhModule &phModule, TempModule &tempModule);

    void begin();
    void loop();

private:
    void displayMenu();
    void displayTemperature();
    LiquidCrystal_I2C _lcd;
    RotaryEncoder _encoder;
    const char *_menuItems[3] = {"tds value", "temp value", "ph value"};
    int _menuPosition = 0;
    int _prevMenuPosition = 0;
    TDSModule _tdsModule;
    PhModule _phModule;
    TempModule _tempModule;
};

#endif
