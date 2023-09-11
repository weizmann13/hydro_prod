#ifndef DISPLAYMODULE_H
#define DISPLAYMODULE_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>
#include <PCF8575.h>

class DisplayModule
{
public:
    DisplayModule(PCF8575 &pcf8575);

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
    void disposelTank();
    // Function to append a MenuItem to _currentTree
    void appendMenuItem(MenuItem *menuItem);
    int changeRelay();

    // Function to remove the last MenuItem from _currentTree
    void popMenuItem();
    void relayOn();
    void relayOff();
    void goBack();
    PCF8575 &_pcf8575;
    LiquidCrystal_I2C _lcd;
    RotaryEncoder _encoder;
    MenuItem *_currentMenu;
    bool _inFunction = false;
    int _currentMenuItemIndex = 0;
    int _scrollIndex = 0;
    int _currentRelay = 0;
    int _prevCurrentMenuItemIndex = 0;
    bool _inDisplay = false;
    bool _buttonState = false;
    unsigned long _displayStartTime = 0;
    unsigned long _funcStartTime = 0;
    unsigned long _lastDispayTime = 0;
    long _encoderPosition = 0;
    long _prevEncoderPosition = 0;
    int _disposelValve = 0;

    // Define your root menu with submenus
    MenuItem _rootMenu = {"Root Menu", _subMenus, 6, nullptr};

    // Define your menu tree structure
    MenuItem _subMenus[7] = {
        {"Relays", _relaysSubMenu, 9, nullptr},
        {"Sensors", _sensorsSubMenu, 4, nullptr},
        {"Actions", _actionsSubMenu, 2, nullptr}, // Attach submenus later
        {"Calibrations", nullptr, 0, nullptr},    // Attach submenus later
        {"Sensors2", _sensorsSubMenu, 4, nullptr},
        {"Actions2", nullptr, 0, nullptr},     // Attach submenus later
        {"Calibrations2", nullptr, 0, nullptr} // Attach submenus later
    };

    // Define your submenu options

    MenuItem _actionsSubMenu[2] = {
        {"go back", nullptr, 0, &DisplayModule::goBack},
        {"Disposal Tank", nullptr, 0, &DisplayModule::disposelTank}};

    MenuItem _sensorsSubMenu[4] = {
        {"go back", nullptr, 0, &DisplayModule::goBack},
        {"TDS", nullptr, 0, &DisplayModule::displayTds},
        {"pH", nullptr, 0, &DisplayModule::displayPh},
        {"Temperature", nullptr, 0, &DisplayModule::displayTemperature}};

    MenuItem _relaysSubMenu[9] = {
        {"go back", nullptr, 0, &DisplayModule::goBack},
        {"relay 0", _relayMenu, 3, nullptr},
        {"relay 1", _relayMenu, 3, nullptr},
        {"relay 2", _relayMenu, 3, nullptr},
        {"relay 3", _relayMenu, 3, nullptr},
        {"relay 4", _relayMenu, 3, nullptr},
        {"relay 5", _relayMenu, 3, nullptr},
        {"relay 7", _relayMenu, 3, nullptr},
        {"relay 8", _relayMenu, 3, nullptr}};

    MenuItem _relayMenu[3]{
        {"go back", nullptr, 0, &DisplayModule::goBack},
        {"Relay on", nullptr, 0, &DisplayModule::relayOn},
        {"Relay of", nullptr, 0, &DisplayModule::relayOff}};

    // Define your other submenus similarly
    MenuItem _goBackSubMenu = {"Go Back", nullptr, 0, &DisplayModule::goBack};

    MenuItem **_currentTree;
    int _currentTreeSize = 1; // Current size of _currentTree
};
#endif
