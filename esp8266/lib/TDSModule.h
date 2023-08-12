#ifndef TDSMODULE_H
#define TDSMODULE_H

#include <Arduino.h>
#include "TempModule.h"

class TDSModule
{
public:
    TDSModule(int tdsPin, int powerPin, int tempPin);
    TDSModule(int tdsPin, int tdsPowerPin, int tempPin, float VREF);
    TDSModule(int tdsPin, int tdsPowerPin, int tempPin, int SCOUNT);
    TDSModule(int tdsPin, int tdsPowerPin, int tempPin, float VREF, int SCOUNT);

    void begin();
    int readSensor();
    int getValue();

private:
    int getMedianNum(int bArray[], int iFilterLen);
    int readTds();

    int _tdsPin;
    int _tdsPowerPin;
    int _tempPin;
    int _tdsValue;
    float _VREF = 3.3;
    int _SCOUNT = 30;
    TempModule _tempModule;
};

#endif