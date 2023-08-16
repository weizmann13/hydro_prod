#include "DisplayModule.h"
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>
#include "TDSModule.h"  // Class for TDS sensor
#include "PhModule.h"   // Class for pH sensor
#include "TempModule.h" // Class for temperature sensor

DisplayModule::DisplayModule(TDSModule &tdsModule, PhModule &phModule, TempModule &tempModule)
    : _lcd(0x27, 20, 4), _encoder(D5, D6), _tdsModule(tdsModule), _phModule(phModule), _tempModule(tempModule)
{
}

void DisplayModule::displayMenu()
{
    _lcd.clear();
    for (int i = 0; i < 3; i++)
    {
        if (i == _menuPosition)
        {
            _lcd.print("> ");
        }
        else
        {
            _lcd.print("  ");
        }
        _lcd.print(_menuItems[i]);
        _lcd.setCursor(0, i + 1);
    }
}

void DisplayModule::displayTemperature()
{
    _lcd.clear();
    _lcd.print("Temp: ");
    _lcd.print(_tempModule.getValue());
    _lcd.print(" C");
}

void DisplayModule::displayTds()
{
    _lcd.clear();
    _lcd.print("Tds: ");
    _lcd.print(_tdsModule.getValue());
    _lcd.print(" ppm");
}

void DisplayModule::displayPh()
{
    _lcd.clear();
    _lcd.print("Ph: ");
    _lcd.print(_phModule.getValue());
    _lcd.print(" somthing");
}

void DisplayModule::loop()
{
    if (_inDisplay && millis() - _displayStartTime > 3000)
    {
        _inDisplay = false;
        displayMenu();
    }
    else if (_inDisplay)
    {
        return;
    }
    _encoder.tick();
    // Update the menu position
    _menuPosition = _encoder.getPosition() % 3;
    if (_menuPosition < 0)
    {
        _menuPosition += 3;
    }

    // Check if the menu position has changed
    if (_menuPosition != _prevMenuPosition)
    {
        displayMenu();
        _prevMenuPosition = _menuPosition;
    }
    // Check if the rotary encoder button is pressed
    if (digitalRead(D7) == LOW)
    {
        _inDisplay = true;
        _displayStartTime = millis();
        // Perform an action based on the selected menu item
        switch (_menuPosition)
        {
        case 0:
            displayTds();
            break;
        case 1:
            // Action for Option 2
            displayTemperature();
            break;
        case 2:
            // Action for Option 3
            displayPh();
            break;
        }
    }
}

void DisplayModule::begin()
{
    _lcd.init();
    _lcd.backlight();
    pinMode(D7, INPUT_PULLUP);
    _encoder.setPosition(0);
    displayMenu();
}