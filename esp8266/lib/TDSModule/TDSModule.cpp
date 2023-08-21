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

float TDSModule::getAverageNum(int bArray[], int iFilterLen)
{
    float sum = 0;
    for (int i = 0; i < iFilterLen; i++)
    {
        sum += bArray[i];
    }
    return sum / iFilterLen;
}

void TDSModule::begin()
{
    pinMode(_tdsPowerPin, OUTPUT);
    digitalWrite(_tdsPowerPin, LOW);
    _tempModule.begin();
    _analogBuffer = new int[_SCOUNT];
}

float TDSModule::readTds()
{
    int tempV = analogRead(_tdsPin);
    float averageVoltage = tempV * (float)_VREF / 1024.0;
    float temperatureC = _tempModule.readSensor();
    float compensationCoefficient = 1.0 + 0.02 * (temperatureC - 25.0);
    float compensationVoltage = averageVoltage / compensationCoefficient;
    return (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5;

    // if (millis() - _previousMillis > 40U)
    // {
    //     _previousMillis = millis();
    //     _analogBuffer[_bufferCounter] = analogRead(_tdsPin); // read the analog value and store into the buffer
    //     if (_bufferCounter == _SCOUNT)
    //     {
    //         float averageVoltage = getAverageNum(_analogBuffer, _SCOUNT) * (float)_VREF / 1024.0;
    //         float temperatureC = _tempModule.readSensor();
    //         float compensationCoefficient = 1.0 + 0.02 * (temperatureC - 25.0);
    //         float compensationVoltage = averageVoltage / compensationCoefficient;
    //         _bufferCounter = 0;
    //         return (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5;
    //     }
    //     _bufferCounter += 1;
    // }
    // return 0;
}

float TDSModule::readSensor()
{
    digitalWrite(_tdsPowerPin, HIGH);
    delay(3);
    float tmp_value = readTds();
    digitalWrite(_tdsPowerPin, LOW);
    return tmp_value;
    // if (tmp_value)
    // {
    //     _tdsValue = tmp_value;
    //     return true;
    // }
    // return false;
}

float TDSModule::getValue()
{
    return _tdsValue;
}