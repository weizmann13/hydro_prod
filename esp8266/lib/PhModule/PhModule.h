#ifndef PHMODULE_H
#define PHMODULE_H

#include "Adafruit_ADS1X15.h"

class PhModule
{
public:
    PhModule(int phAdsPin, Adafruit_ADS1115 &ads);
    PhModule(int phAdsPin, Adafruit_ADS1115 &ads, float calibration_value);
    PhModule(int phAdsPin, Adafruit_ADS1115 &ads, float calibration_value, float VREF);
    PhModule(int phAdsPin, Adafruit_ADS1115 &ads, float calibration_value, float VREF, int SCOUNT);

    void begin();
    bool readSensor();
    float getValue();

private:
    float readPh();
    float getAverageNum(int bArray[], int iFilterLen);

    int _phAdsPin;
    Adafruit_ADS1115 &_ads;
    float _calibration_value = 22.84 - 0.7;
    float _VREF = 3.3;
    int _SCOUNT = 10;
    float _value = 0.0;
    unsigned long _previousMillis = 0;
    int *_analogBuffer;
    int _bufferCounter = 0;
};

#endif