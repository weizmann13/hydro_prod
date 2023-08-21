#ifndef PHMODULE_H
#define PHMODULE_H

class PhModule
{
public:
    PhModule(int pin, int powerPin);
    PhModule(int pin, int powerPin, float calibration_value);
    PhModule(int pin, int powerPin, float calibration_value, float VREF);
    PhModule(int pin, int powerPin, float calibration_value, float VREF, int SCOUNT);

    void begin();
    float readSensor();
    float getValue();

private:
    float readPh();
    float getAverageNum(int bArray[], int iFilterLen);

    int _pin;
    int _powerPin;
    float _calibration_value = 22.84 - 0.7;
    unsigned long _previousMillis = 0;
    int *_analogBuffer;
    int _bufferCounter = 0;
    int _SCOUNT = 20;
    float _value;
    float _VREF = 3.3;
};

#endif