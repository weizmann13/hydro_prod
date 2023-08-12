#ifndef PHMODULE_H
#define PHMODULE_H

class PhModule
{
public:
    PhModule(int pin, int powerPin);
    PhModule(int pin, int powerPin, float calibration_value);
    PhModule(int pin, int powerPin, float calibration_value, float VREF);

    void begin();
    float readSensor();
    float getValue();

private:
    float readPh();

    int _pin;
    int _powerPin;
    float _value;
    float _calibration_value = 22.84 - 0.7;
    float _VREF = 3.3;
};

#endif