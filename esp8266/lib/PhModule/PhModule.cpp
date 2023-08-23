#include "PhModule.h"
#include <Arduino.h>

PhModule::PhModule(int phAdsPin, Adafruit_ADS1115 &ads)
    : _phAdsPin(phAdsPin), _ads(ads)
{
}

PhModule::PhModule(int phAdsPin, Adafruit_ADS1115 &ads, float calibration_value)
    : _phAdsPin(phAdsPin), _ads(ads), _calibration_value(calibration_value)
{
}

PhModule::PhModule(int phAdsPin, Adafruit_ADS1115 &ads, float calibration_value, float VREF)
    : _phAdsPin(phAdsPin), _ads(ads), _calibration_value(calibration_value), _VREF(VREF)
{
}

PhModule::PhModule(int phAdsPin, Adafruit_ADS1115 &ads, float calibration_value, float VREF, int SCOUNT)
    : _phAdsPin(phAdsPin), _ads(ads), _calibration_value(calibration_value), _VREF(VREF), _SCOUNT(SCOUNT)
{
}

float PhModule::getAverageNum(int bArray[], int iFilterLen)
{
    float sum = 0;
    for (int i = 0; i < iFilterLen; i++)
    {
        sum += bArray[i];
    }
    return sum / iFilterLen;
}

float PhModule::readPh()
{
    if (millis() - _previousMillis > 40U)
    {
        // Serial.print("Ph Buffer: ");
        // Serial.println(_analogBuffer[_bufferCounter]);
        if (_bufferCounter == _SCOUNT)
        {
            float avgval = _ads.computeVolts(getAverageNum(_analogBuffer, _SCOUNT));
            // Serial.print("Ph Voltage: ");
            // Serial.println(avgval);
            float volt = (float)avgval / 6;
            float ph_act = -5.70 * volt + _calibration_value;
            _bufferCounter = 0;
            return ph_act;
        }
        _analogBuffer[_bufferCounter] = _ads.readADC_SingleEnded(_phAdsPin);
        _previousMillis = millis();
        _bufferCounter += 1;
    }
    return 0;
}

void PhModule::begin()
{
    _analogBuffer = new int[_SCOUNT];
}

bool PhModule::readSensor()
{
    float tmp_value = readPh();
    if (tmp_value)
    {
        _value = tmp_value;
        return true;
    }
    return false;
}

float PhModule::getValue()
{
    // Read analog value from _pin and perform sensor-specific calculations
    // to obtain TDS value
    return _value;
}