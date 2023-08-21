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

PhModule::PhModule(int pin, int powerPin, float calibration_value, float VREF)
    : _pin(pin), _value(0.0), _powerPin(powerPin), _calibration_value(calibration_value), _VREF(VREF)
{
}

PhModule::PhModule(int pin, int powerPin, float calibration_value, float VREF, int SCOUNT)
    : _pin(pin), _value(0.0), _powerPin(powerPin), _calibration_value(calibration_value), _VREF(VREF), _SCOUNT(SCOUNT)
{
}

float PhModule::getAverageNum(int bArray[], int iFilterLen)
{
    float sum = 0;
    for (int i = 0; i < iFilterLen; i++)
    {
        sum += bArray[i];
    }
    return sum / iFilterLen;
}

float PhModule::readPh()
{
    int tempV = analogRead(_pin);
    float volt = tempV * _VREF / 1024 / 6;
    float ph_act = -5.70 * volt + _calibration_value;
    return ph_act;

    // if (millis() - _previousMillis > 40U)
    // {
    //     _previousMillis = millis();
    //     _analogBuffer[_bufferCounter] = analogRead(_pin);
    //     if (_bufferCounter == _SCOUNT)
    //     {
    //         float avgval = getAverageNum(_analogBuffer, _SCOUNT);
    //         float volt = (float)avgval * _VREF / 1024 / 6;
    //         float ph_act = -5.70 * volt + _calibration_value;
    //         _bufferCounter = 0;
    //         return ph_act;
    //     }
    //     _bufferCounter += 1;
    // }
    // return 0;
}

void PhModule::begin()
{
    pinMode(_powerPin, OUTPUT);
    digitalWrite(_powerPin, LOW);
    _analogBuffer = new int[_SCOUNT];
}

float PhModule::readSensor()
{
    digitalWrite(_powerPin, HIGH);
    delay(3);
    float tmp_value = readPh();
    digitalWrite(_powerPin, LOW);
    return tmp_value;
    // if (tmp_value)
    // {
    //     _value = tmp_value;
    //     return true;
    // }
    // return false;
}

float PhModule::getValue()
{
    // Read analog value from _pin and perform sensor-specific calculations
    // to obtain TDS value
    return _value;
}