#include "DisplayModule.h"

DisplayModule::DisplayModule(PCF8575 &pcf8575)
    : _pcf8575(pcf8575), _lcd(0x27, 20, 4), _encoder(D5, D6), _currentTree(new MenuItem *[1]), _currentTreeSize(1)
{
    _currentTree[0] = &_rootMenu;
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

void DisplayModule::displayTemperature()
{
    _lcd.clear();
    _lcd.print("Temp: ");
    // _lcd.print(_tempModule.getValue());
    _lcd.print(" C");
}

void DisplayModule::displayTds()
{
    _lcd.clear();
    _lcd.print("Tds: ");
    // _lcd.print(_tdsModule.getValue());
    _lcd.print(" ppm");
}

void DisplayModule::displayPh()
{
    _lcd.clear();
    _lcd.print("Ph: ");
    // _lcd.print(_phModule.getValue());
    _lcd.print(" somthing");
}

void DisplayModule::loop()
{
    if (_inFunction)
    {
        Serial.println("in function");
        MenuItem *selectedMenuItem = &_currentMenu->subMenus[_currentMenuItemIndex];
        (this->*selectedMenuItem->function)();
    }
    else
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
int DisplayModule::changeRelay()
{
    int num = _currentMenu->name[6] - '0';
    Serial.println(num);
    return num;
}
void DisplayModule::handleEncoderButton()
{

    // Perform an action based on the selected menu item
    MenuItem *selectedMenuItem = &_currentMenu->subMenus[_currentMenuItemIndex];
    // Check if the selected menu item is a function or a submenu
    if (selectedMenuItem->numSubMenus == 0)
    {
        (this->*selectedMenuItem->function)();
    }
    else
    {
        if (selectedMenuItem->function != nullptr)
        {
            (this->*selectedMenuItem->function)();
        }
        _currentMenu = selectedMenuItem;
        appendMenuItem(selectedMenuItem);
        Serial.println(_currentTree[_currentTreeSize - 1]->name);
        _currentMenuItemIndex = 0;
        _scrollIndex = 0;
        displayMenu();
    }
}
void DisplayModule::appendMenuItem(MenuItem *menuItem)
{
    // Create a new array with increased size
    MenuItem **newTree = new MenuItem *[_currentTreeSize + 1];

    // Copy existing pointers
    for (int i = 0; i < _currentTreeSize; ++i)
    {
        newTree[i] = _currentTree[i];
    }

    // Append the new menuItem
    newTree[_currentTreeSize] = menuItem;

    // Delete old array and update the pointer
    delete[] _currentTree;
    _currentTree = newTree;

    // Increment the size
    ++_currentTreeSize;
}

void DisplayModule::relayOn()
{
    int pinNumber = changeRelay();
    _pcf8575.digitalWrite(pinNumber, LOW);
}

void DisplayModule::relayOff()
{
    int pinNumber = changeRelay();
    _pcf8575.digitalWrite(pinNumber, HIGH);
}

void DisplayModule::disposelTank()
{
    if (!_inFunction)
    {
        _funcStartTime = millis();
        _inFunction = true;
        _pcf8575.digitalWrite(_disposelValve, LOW);
        _lcd.clear();
        _lcd.print("Disposel Tank");
        _lcd.setCursor(0, 1);
        _lcd.print("time left: 5s");

        return;
    }
    if (millis() - _lastDispayTime > 500)
    {
        _lcd.clear();
        _lcd.print("Disposel Tank");
        _lcd.setCursor(0, 1);
        _lcd.printf("time left: %.2fs", (5000 - (millis() - _funcStartTime)) / 1000.0);
        _lastDispayTime = millis();
    }
    if (millis() - _funcStartTime > 5000)
    {
        _pcf8575.digitalWrite(_disposelValve, HIGH);
        _inFunction = false;
        goBack();
    }
}
void DisplayModule::goBack()
{
    // Check if the current menu is the root menu
    if (_currentMenu == &_rootMenu)
    {
        return;
    }

    _currentMenu = _currentTree[_currentTreeSize - 2];
    // Check if the current menu is the first submenu of the root menu
    popMenuItem();
    // Set the current menu to the parent menu
    _currentMenuItemIndex = 0;
    _scrollIndex = 0;
    displayMenu();
}

// Implement the popMenuItem function
void DisplayModule::popMenuItem()
{
    if (_currentTreeSize > 0)
    {
        Serial.println("Popping menu item");
        // Create a new array with reduced size
        MenuItem **newTree = new MenuItem *[_currentTreeSize - 1];
        Serial.println("2Popping menu item");
        // Copy existing pointers
        for (int i = 0; i < _currentTreeSize - 1; ++i)
        {
            newTree[i] = _currentTree[i];
            Serial.println("3Popping menu item");
        }

        // Delete old array and update the pointer
        delete[] _currentTree;
        Serial.println("4Popping menu item");
        _currentTree = newTree;

        // Decrement the size
        --_currentTreeSize;
    }
}
void DisplayModule::begin()
{
    for (int i = 0; i < 8; i++)
    {
        _pcf8575.digitalWrite(i, HIGH);
    }
    _lcd.init();
    _lcd.backlight();
    pinMode(D7, INPUT_PULLUP);
    _encoder.setPosition(0);
    _currentMenu = &_rootMenu;
    displayMenu();
}