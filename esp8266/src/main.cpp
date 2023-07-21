#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>
#include <Ticker.h>
#include <Time.h>

#define TdsSensorPin A0
#define VREF 3.3  // analog reference voltage(Volt) of the ADC
#define SCOUNT 30 // sum of sample point

// wifi configuration
#define WIFI_SSID "Ayasofer"
#define WIFI_PASSWORD "02742151"

// mqtt server configuration10.100.102.21
#define MQTT_HOST IPAddress(10, 100, 102, 21)
#define MQTT_PORT 1883
#define MQTT_PUB_TDS "Hydro/tds"

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

float temperature = 23; // current temperature for compensation

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
  int tmp_tds = readTds();
  Serial.println(tmp_tds);
  Serial.println(analogRead(A0));
  char influxDBLineProtocol[128]; // Adjust buffer size as per your requirements
  sprintf(influxDBLineProtocol, "tds,tds_sensor=sensor_1 tds_value=%d", tmp_tds);
  uint16_t packetIdPub1 = mqttClient.publish(MQTT_PUB_TDS, 2, true, influxDBLineProtocol);
  Serial.print("Publishing at QoS 2, packetId: ");
  Serial.println(packetIdPub1);
  delay(1000);
}
