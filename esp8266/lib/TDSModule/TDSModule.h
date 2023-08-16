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
    bool readSensor();
    float getValue();

private:
    float getAverageNum(int bArray[], int iFilterLen);
    float readTds();

    unsigned long _previousMillis = 0;
    int *_analogBuffer;
    int _bufferCounter = 0;
    int _tdsPin;
    int _tdsPowerPin;
    int _tempPin;
    int _tdsValue;
    float _VREF = 3.3;
    int _SCOUNT = 20;
    TempModule _tempModule;
};

#endif