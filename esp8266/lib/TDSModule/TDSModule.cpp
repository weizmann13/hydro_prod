#include "TDSModule.h"
#include <Arduino.h>
#include "TempModule.h"

TDSModule::TDSModule(int tdsPin, int tdsPowerPin, int tempPin)
    : _tdsPin(tdsPin), _tdsPowerPin(tdsPowerPin), _tempPin(tempPin), _tdsValue(0),
      _tempModule(tempPin)
{
}

TDSModule::TDSModule(int tdsPin, int tdsPowerPin, int tempPin, float VREF)
    : _tdsPin(tdsPin), _tdsPowerPin(tdsPowerPin), _tempPin(tempPin), _tdsValue(0),
      _VREF(VREF), _tempModule(tempPin)
{
}

TDSModule::TDSModule(int tdsPin, int tdsPowerPin, int tempPin, int SCOUNT)
    : _tdsPin(tdsPin), _tdsPowerPin(tdsPowerPin), _tempPin(tempPin), _tdsValue(0),
      _SCOUNT(SCOUNT), _tempModule(tempPin)
{
}

TDSModule::TDSModule(int tdsPin, int tdsPowerPin, int tempPin, float VREF, int SCOUNT)
    : _tdsPin(tdsPin), _tdsPowerPin(tdsPowerPin), _tempPin(tempPin), _tdsValue(0),
      _VREF(VREF), _SCOUNT(SCOUNT), _tempModule(tempPin)
{
}

int TDSModule::getMedianNum(int bArray[], int iFilterLen)
{
    int bTab[iFilterLen];
    for (byte i = 0; i < iFilterLen; i++)
        bTab[i] = bArray[i];
    int i, j, bTemp;
    for (j = 0; j < iFilterLen - 1; j++)
    {
        for (i = 0; i < iFilterLen - j - 1; i++)
        {
            if (bTab[i] > bTab[i + 1])
            {
                bTemp = bTab[i];
                bTab[i] = bTab[i + 1];
                bTab[i + 1] = bTemp;
            }
        }
    }
    if ((iFilterLen & 1) > 0)
    {
        bTemp = bTab[(iFilterLen - 1) / 2];
    }
    else
    {
        bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
    }
    return bTemp;
}

void TDSModule::begin()
{
    pinMode(_tdsPowerPin, OUTPUT);
    digitalWrite(_tdsPowerPin, LOW);
    _tempModule.begin();
}

int TDSModule::readTds()
{
    int analogBuffer[_SCOUNT];
    static unsigned long analogSampleTimepoint = millis();
    for (int i = 0; i < _SCOUNT; i++)
    {
        analogSampleTimepoint = millis();
        analogBuffer[i] = analogRead(_tdsPin); // read the analog value and store into the buffer
        delay(40);
    }

    float averageVoltage = getMedianNum(analogBuffer, _SCOUNT) * (float)_VREF / 1024.0;
    float temperatureC = _tempModule.readSensor();
    float compensationCoefficient = 1.0 + 0.02 * (temperatureC - 25.0);

    float compensationVoltage = averageVoltage / compensationCoefficient;

    float tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5;

    return tdsValue;
}

int TDSModule::readSensor()
{
    digitalWrite(_tdsPowerPin, HIGH);
    delay(10);
    _tdsValue = readTds();
    digitalWrite(_tdsPowerPin, LOW);
    return _tdsValue;
}

int TDSModule::getValue()
{
    return _tdsValue;
}