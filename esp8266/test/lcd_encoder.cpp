#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);
// Initialize the rotary encoder
RotaryEncoder encoder(D5, D6);

const char *menuItems[] = {"Option 1", "Option 2", "Option 3", "Temp & Humidity"};
// Variables to store the current menu position and previous position
int menuPosition = 0;
int prevMenuPosition = 0;

void displayMenu()
{
    lcd.clear();
    for (int i = 0; i < 4; i++)
    {
        if (i == menuPosition)
        {
            lcd.print("> ");
        }
        else
        {
            lcd.print("  ");
        }
        lcd.print(menuItems[i]);
        lcd.setCursor(0, i + 1);
    }
}
void displayTemperatureAndHumidity()
{
    lcd.clear();
    lcd.print("Temp: ");
    lcd.print(" C");
    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    lcd.print(" %");
    delay(2000);   // Display the readings for 2 seconds
    displayMenu(); // Return to the menu
}

void setup()
{
    // Initialize the LCD display
    lcd.init();
    lcd.backlight();
    // Initialize the rotary encoder
    pinMode(D7, INPUT_PULLUP);
    encoder.setPosition(0);
    // Initialize the DHT11 sensor
    // Display the menu
    displayMenu();
}
void loop()
{
    // Read the rotary encoder
    encoder.tick();
    // Update the menu position
    menuPosition = encoder.getPosition() % 4;
    if (menuPosition < 0)
    {
        menuPosition += 4;
    }

    // Check if the menu position has changed
    if (menuPosition != prevMenuPosition)
    {
        displayMenu();
        prevMenuPosition = menuPosition;
    }
    // Check if the rotary encoder button is pressed
    if (digitalRead(D7) == LOW)
    {
        // Perform an action based on the selected menu item
        switch (menuPosition)
        {
        case 0:
            // Action for Option 1
            break;
        case 1:
            // Action for Option 2
            break;
        case 2:
            // Action for Option 3
            break;
        case 3:
            // Action for displaying temperature and humidity
            displayTemperatureAndHumidity();
            break;
        }
    }
}
