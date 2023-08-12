#include "PhModule.h"
#include <Arduino.h>

PhModule::PhModule(int pin, int powerPin)
    : _pin(pin), _value(0.0), _powerPin(powerPin)
{
}

PhModule::PhModule(int pin, int powerPin, float calibration_value)
    : _pin(pin), _value(0.0), _powerPin(powerPin), _calibration_value(calibration_value)
{
}

PhModule::PhModule(int pin, int powerPin, float VREF)
    : _pin(pin), _value(0.0), _powerPin(powerPin), _VREF(VREF)
{
}

PhModule::PhModule(int pin, int powerPin, float calibration_value, float VREF)
    : _pin(pin), _value(0.0), _powerPin(powerPin), _calibration_value(calibration_value), _VREF(VREF)
{
}

float PhModule::readPh()
{
    int buffer_arr[10], temp;
    float ph_act;
    unsigned long int avgval;
    for (int i = 0; i < 10; i++)
    {
        buffer_arr[i] = analogRead(_pin);
        delay(30);
    }
    for (int i = 0; i < 9; i++)
    {
        for (int j = i + 1; j < 10; j++)
        {
            if (buffer_arr[i] > buffer_arr[j])
            {
                temp = buffer_arr[i];
                buffer_arr[i] = buffer_arr[j];
                buffer_arr[j] = temp;
            }
        }
    }
    avgval = 0;
    for (int i = 2; i < 8; i++)
        avgval += buffer_arr[i];
    float volt = (float)avgval * _VREF / 1024 / 6;
    ph_act = -5.70 * volt + _calibration_value;
    return ph_act;
}

void PhModule::begin()
{
}

float PhModule::readSensor()
{
    // Read analog value from _pin and perform sensor-specific calculations
    // to obtain TDS value
    _value = readPh();
    return _value;
}

float PhModule::getValue()
{
    // Read analog value from _pin and perform sensor-specific calculations
    // to obtain TDS value
    return _value;
}