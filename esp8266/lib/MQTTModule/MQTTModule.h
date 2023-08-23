// MQTTModule.h
#ifndef MQTTMODULE_H
#define MQTTMODULE_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>
#include <Ticker.h>

class MQTTModule
{
public:
    MQTTModule(const char *ssid, const char *password, IPAddress mqttHost, int mqttPort);
    void begin();
    void publishData(const char *topic, float tdsValue, float phValue, float tempValue, float ecValue);

private:
    void connectToWifi();
    void connectToMqtt();
    void onWifiConnect(const WiFiEventStationModeGotIP &event);
    void onWifiDisconnect(const WiFiEventStationModeDisconnected &event);
    void onMqttConnect(bool sessionPresent);
    void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
    void onMqttPublish(uint16_t packetId);
    void publish(const char *topic, const char *message);

    const char *_ssid;
    const char *_password;
    IPAddress _mqttHost;
    int _mqttPort;
    AsyncMqttClient _mqttClient;
    WiFiEventHandler _wifiConnectHandler;
    WiFiEventHandler _wifiDisconnectHandler;
    Ticker _mqttReconnectTimer;
    Ticker _wifiReconnectTimer;
};

#endif
