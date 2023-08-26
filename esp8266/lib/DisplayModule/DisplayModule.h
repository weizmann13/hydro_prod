#ifndef DISPLAYMODULE_H
#define DISPLAYMODULE_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>

class DisplayModule
{
public:
    DisplayModule();

    void begin();
    void loop();

private:
    struct MenuItem
    {
        const char *name;
        MenuItem *subMenus;
        int numSubMenus;
        void (*function)(LiquidCrystal_I2C &_lcd);
    };

    void displayMenu();
    void updateMenuPosition();
    void handleEncoderButton();

    static void displayTemperature(LiquidCrystal_I2C &_lcd);
    static void displayTds(LiquidCrystal_I2C &_lcd);
    static void displayPh(LiquidCrystal_I2C &_lcd);

    LiquidCrystal_I2C _lcd;
    RotaryEncoder _encoder;
    MenuItem *_currentMenu;
    int _currentMenuItemIndex = 0;
    int _scrollIndex = 0;
    int _prevCurrentMenuItemIndex = 0;
    bool _inDisplay = false;
    bool _buttonState = false;
    unsigned long _displayStartTime = 0;
    long _encoderPosition = 0;
    long _prevEncoderPosition = 0;

    // Define your root menu with submenus
    MenuItem _rootMenu = {"Root Menu", _subMenus, 6, nullptr};

    // Define your menu tree structure
    MenuItem _subMenus[6] = {
        {"Sensors", _sensorsSubMenu, 3, nullptr},
        {"Actions", nullptr, 0, nullptr},      // Attach submenus later
        {"Calibrations", nullptr, 0, nullptr}, // Attach submenus later
        {"Sensors2", _sensorsSubMenu, 3, nullptr},
        {"Actions2", nullptr, 0, nullptr},     // Attach submenus later
        {"Calibrations2", nullptr, 0, nullptr} // Attach submenus later
    };

    // Define your submenu options
    MenuItem _sensorsSubMenu[3] = {
        {"TDS", nullptr, 0, &displayTds},
        {"pH", nullptr, 0, &displayPh},
        {"Temperature", nullptr, 0, &displayTemperature}};

    // Define your other submenus similarly
    MenuItem _goBackSubMenu = {"Go Back", nullptr, 0, nullptr};

    MenuItem *_currentTree[1] = {&_rootMenu};
};
#endif
