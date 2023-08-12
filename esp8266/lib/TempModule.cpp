#include "TempModule.h"
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

TempModule::TempModule(int pin)
    : _pin(pin), _value(0.0), _oneWire(pin), _sensors(&_oneWire)
{
}

void TempModule::begin()
{
    _sensors.begin();
}

float TempModule::readSensor()
{
    // Read analog value from _pin and perform sensor-specific calculations
    // to obtain TDS value
    _sensors.requestTemperatures();
    float temperatureC = _sensors.getTempCByIndex(0);
    _value = temperatureC;
    return _value;
}

float TempModule::getValue()
{
    // Read analog value from _pin and perform sensor-specific calculations
    // to obtain TDS value
    return _value;
}