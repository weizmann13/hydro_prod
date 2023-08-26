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
        void (DisplayModule::*function)();
    };

    void displayMenu();
    void updateMenuPosition();
    void handleEncoderButton();

    void displayTemperature();
    void displayTds();
    void displayPh();

    // Function to append a MenuItem to _currentTree
    void appendMenuItem(MenuItem *menuItem);
    // Function to remove the last MenuItem from _currentTree
    void popMenuItem();

    void goBack();
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
        {"Sensors", _sensorsSubMenu, 4, nullptr},
        {"Actions", nullptr, 0, nullptr},      // Attach submenus later
        {"Calibrations", nullptr, 0, nullptr}, // Attach submenus later
        {"Sensors2", _sensorsSubMenu, 4, nullptr},
        {"Actions2", nullptr, 0, nullptr},     // Attach submenus later
        {"Calibrations2", nullptr, 0, nullptr} // Attach submenus later
    };

    // Define your submenu options

    MenuItem _sensorsSubMenu[4] = {
        {"go back", nullptr, 0, &DisplayModule::goBack},
        {"TDS", nullptr, 0, &DisplayModule::displayTds},
        {"pH", nullptr, 0, &DisplayModule::displayPh},
        {"Temperature", nullptr, 0, &DisplayModule::displayTemperature}};

    // Define your other submenus similarly
    MenuItem _goBackSubMenu = {"Go Back", nullptr, 0, &DisplayModule::goBack};

    MenuItem **_currentTree;
    int _currentTreeSize = 1; // Current size of _currentTree
};
#endif
