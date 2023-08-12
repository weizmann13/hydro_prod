#ifndef TEMPMODULE_H
#define TEMPMODULE_H

#include <OneWire.h>
#include <DallasTemperature.h>

class TempModule
{
public:
    TempModule(int pin);
    void begin();
    float readSensor();
    float getValue();

private:
    int _pin;
    float _value;
    OneWire _oneWire;
    DallasTemperature _sensors;
};

#endif