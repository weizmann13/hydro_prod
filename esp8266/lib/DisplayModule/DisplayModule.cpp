#include "DisplayModule.h"

DisplayModule::DisplayModule()
    : _lcd(0x27, 20, 4), _encoder(D5, D6)
{
}

void DisplayModule::displayMenu()
{
    // check if the menu requires scrolling
    if (_currentMenu->numSubMenus > 4)
    {
        if (_currentMenuItemIndex > _scrollIndex + 3)
        {
            _scrollIndex++;
        }
        else if (_currentMenuItemIndex < _scrollIndex)
        {
            _scrollIndex--;
        }
    }
    else
    {
        _scrollIndex = 0;
    }

    _lcd.clear();
    int maxDisplayRows = 4; // Number of rows to display
    int loopIterations = min(maxDisplayRows, _currentMenu->numSubMenus);
    for (int i = 0; i < loopIterations; i++)
    {
        if (i == _currentMenuItemIndex - _scrollIndex)
        {
            _lcd.print("> ");
        }
        else
        {
            _lcd.print("  ");
        }
        _lcd.print(_currentMenu->subMenus[i + _scrollIndex].name);
        _lcd.setCursor(0, i + 1);
    }
}

void DisplayModule::displayTemperature(LiquidCrystal_I2C &_lcd)
{
    _lcd.clear();
    _lcd.print("Temp: ");
    // _lcd.print(_tempModule.getValue());
    _lcd.print(" C");
}

void DisplayModule::displayTds(LiquidCrystal_I2C &_lcd)
{
    _lcd.clear();
    _lcd.print("Tds: ");
    // _lcd.print(_tdsModule.getValue());
    _lcd.print(" ppm");
}

void DisplayModule::displayPh(LiquidCrystal_I2C &_lcd)
{
    _lcd.clear();
    _lcd.print("Ph: ");
    // _lcd.print(_phModule.getValue());
    _lcd.print(" somthing");
}

void DisplayModule::loop()
{
    _encoder.tick();
    _encoderPosition = _encoder.getPosition();
    // check if the encoder position has changed
    if (_encoderPosition > _prevEncoderPosition)
    {
        _currentMenuItemIndex = min(_currentMenuItemIndex + 1, _currentMenu->numSubMenus - 1);
    }
    else if (_encoderPosition < _prevEncoderPosition)
    {
        _currentMenuItemIndex = max(_currentMenuItemIndex - 1, 0);
    }
    _prevEncoderPosition = _encoderPosition;
    // Check if the menu has changed
    if (_currentMenuItemIndex != _prevCurrentMenuItemIndex && _currentMenu->numSubMenus > 0)
    {
        displayMenu();
        _prevCurrentMenuItemIndex = _currentMenuItemIndex;
    }
    // Check if the rotary encoder button is pressed
    if (digitalRead(D7) == LOW && !_buttonState)
    {
        _buttonState = true;
        handleEncoderButton();
    }
    else if (digitalRead(D7) == HIGH && _buttonState)
    {
        _buttonState = false;
    }
}

void DisplayModule::handleEncoderButton()
{
    // Perform an action based on the selected menu item
    MenuItem *selectedMenuItem = &_currentMenu->subMenus[_currentMenuItemIndex];
    // Check if the selected menu item is a function or a submenu
    if (selectedMenuItem->numSubMenus == 0)
    {
        selectedMenuItem->function(_lcd);
        _currentMenu = selectedMenuItem;
    }
    else
    {
        _currentMenu = selectedMenuItem;
        _currentMenuItemIndex = 0;
        _scrollIndex = 0;
        displayMenu();
    }
}

void DisplayModule::begin()
{
    _lcd.init();
    _lcd.backlight();
    pinMode(D7, INPUT_PULLUP);
    _encoder.setPosition(0);
    _currentMenu = &_rootMenu;
    displayMenu();
}