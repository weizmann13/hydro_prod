#include <Arduino.h>

#define TdsSensorPin A0
#define VREF 3.3  // analog reference voltage(Volt) of the ADC
#define SCOUNT 30 // sum of sample point

float temperature = 23; // current temperature for compensation

int readAnalogAvarage(int readNum = 10)
{
  int analogSum = 0;
  for (int analogBufferIndex = 0; analogBufferIndex < readNum; analogBufferIndex++)
  {
    analogSum = analogSum + analogRead(A0); // read the analog value and store into the buffer
    delay(1);
  }
  return (analogSum / readNum);
}

int readTds()
{
  int avrageAnalog = readAnalogAvarage();
  // read the analog value more stable by the median filtering algorithm, and convert to voltage value
  float averageVoltage = avrageAnalog * (float)VREF / 1024.0;
  // temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
  float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
  // temperature compensation
  float compensationVoltage = averageVoltage / compensationCoefficient;
  // convert voltage value to tds value
  float tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5;
  return tdsValue;
}
void setup()
{
  Serial.begin(9600);
  pinMode(TdsSensorPin, INPUT);
}
void loop()
{
  Serial.println(readTds());
  delay(100);
}