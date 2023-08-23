#include "TDSModule.h"
#include <Arduino.h>
#include "TempModule.h"

TDSModule::TDSModule(int tdsAdsPin, Adafruit_ADS1115 &ads, TempModule &tempModule)
    : _tdsAdsPin(tdsAdsPin), _ads(ads), _tempModule(tempModule)
{
}

TDSModule::TDSModule(int tdsAdsPin, Adafruit_ADS1115 &ads, TempModule &tempModule, int SCOUNT)
    : _tdsAdsPin(tdsAdsPin), _ads(ads), _tempModule(tempModule), _SCOUNT(SCOUNT)
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
    _analogBuffer = new int[_SCOUNT];
}

float TDSModule::readTds()
{
    if (millis() - _previousMillis > 40U)
    {

        // Serial.print("TDS Buffer: ");
        // Serial.println(_analogBuffer[_bufferCounter]);
        if (_bufferCounter == _SCOUNT)
        {
            float averageVoltage = _ads.computeVolts(getAverageNum(_analogBuffer, _SCOUNT));
            // Serial.print("TDS Voltage: ");
            // Serial.println(averageVoltage);
            float temperatureC = _tempModule.readSensor();
            float compensationCoefficient = 1.0 + 0.02 * (temperatureC - 25.0);
            float compensationVoltage = averageVoltage / compensationCoefficient;
            _bufferCounter = 0;
            float finalTds = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5;
            return finalTds;
        }
        _analogBuffer[_bufferCounter] = _ads.readADC_SingleEnded(_tdsAdsPin); // read the analog value and store into the buffer
        _previousMillis = millis();
        _bufferCounter += 1;
    }
    return 0;
}

bool TDSModule::readSensor()
{
    float tmp_value = readTds();
    if (tmp_value)
    {
        _tdsValue = tmp_value;
        return true;
    }
    return false;
}

float TDSModule::getValue()
{
    return _tdsValue;
}