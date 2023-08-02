#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>
#include <Ticker.h>
#include <Time.h>

#define SensorPin A0
#define VREF 3.3  // analog reference voltage(Volt) of the ADC
#define SCOUNT 30 // sum of sample point

const int TdsPowerPin = 4;
const int PhPowerPin = 5;

float calibration_value = 22.84 - 0.7;
int phval = 0;
unsigned long int avgval;
int buffer_arr[10], temp;

float ph_act;
// wifi configuration
// #define WIFI_SSID "Lonsher-Office"
// #define WIFI_PASSWORD "62306230"
#define WIFI_SSID "Lonsher-Office"
#define WIFI_PASSWORD "62306230"

// mqtt server configuration10.100.102.21
#define MQTT_HOST IPAddress(192, 168, 1, 200)
#define MQTT_PORT 1883
#define MQTT_PUB_TDS "Hydro/esp"

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

int copyIndex = 0;

float averageVoltage = 0;
float tdsValue = 0;
float temperature = 25;

int getMedianNum(int bArray[], int iFilterLen)
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

void connectToWifi()
{
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt()
{
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onWifiConnect(const WiFiEventStationModeGotIP &event)
{
  Serial.println("Connected to Wi-Fi.");
  Serial.println(WiFi.localIP());
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected &event)
{
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, connectToWifi);
}

void onMqttConnect(bool sessionPresent)
{
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected())
  {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos)
{
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId)
{
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
}

void onMqttPublish(uint16_t packetId)
{
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

int readAnalogAvarage(int readNum = 10)
{
  int analogSum = 0;
  for (int analogBufferIndex = 0; analogBufferIndex < readNum; analogBufferIndex++)
  {
    analogSum = analogSum + analogRead(A0); // read the analog value and store into the buffer
    delay(10);
  }
  return (analogSum / readNum);
}

int readTds()
{
  int analogBufferIndex = 0;
  int analogBuffer[SCOUNT];
  static unsigned long analogSampleTimepoint = millis();
  while (analogBufferIndex != SCOUNT)
  {
    if (millis() - analogSampleTimepoint > 40U)
    { // every 40 milliseconds,read the analog value from the ADC
      analogSampleTimepoint = millis();
      analogBuffer[analogBufferIndex] = analogRead(SensorPin); // read the analog value and store into the buffer
      analogBufferIndex++;
    }
  }

  averageVoltage = getMedianNum(analogBuffer, SCOUNT) * (float)VREF / 1024.0;

  float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);

  float compensationVoltage = averageVoltage / compensationCoefficient;

  tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5;

  return tdsValue;
}

float readPh()
{
  for (int i = 0; i < 10; i++)
  {
    buffer_arr[i] = analogRead(A0);
    delay(30);
  }
  for (int i = 0; i < 9; i++)
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buffer_arr[i] > buffer_arr[j])
      {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }
  avgval = 0;
  for (int i = 2; i < 8; i++)
    avgval += buffer_arr[i];
  float volt = (float)avgval * 3.3 / 1024 / 6;
  ph_act = -5.70 * volt + calibration_value;
  return ph_act;
}

void setup()
{
  Serial.begin(9600);
  pinMode(TdsPowerPin, OUTPUT);
  pinMode(PhPowerPin, OUTPUT);
  pinMode(SensorPin, INPUT);
  WiFi.mode(WIFI_STA);
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  // mqttClient.onSubscribe(onMqttSubscribe);
  // mqttClient.onUnsubscribe(onMqttUnsubscribe);
  // mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  connectToWifi();
}

void loop()
{
  digitalWrite(TdsPowerPin, HIGH);
  delay(10);
  int tmp_tds = readTds();
  digitalWrite(TdsPowerPin, LOW);
  digitalWrite(PhPowerPin, HIGH);
  delay(10);
  float tmp_ph = readPh();
  digitalWrite(PhPowerPin, LOW);
  char influxDBLineProtocol[128]; // Adjust buffer size as per your requirements
  sprintf(influxDBLineProtocol, "esp0,hydro_number=0 tds_value=%d,ph_value=%f", tmp_tds, tmp_ph);
  uint16_t packetIdPub1 = mqttClient.publish(MQTT_PUB_TDS, 2, true, influxDBLineProtocol);
  Serial.println(influxDBLineProtocol);
  Serial.print("Publishing at QoS 2, packetId: ");
  Serial.println(packetIdPub1);
  delay(1000);
}