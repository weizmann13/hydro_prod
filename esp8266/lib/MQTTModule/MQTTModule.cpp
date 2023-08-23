// MQTTModule.cpp
#include "MQTTModule.h"

MQTTModule::MQTTModule(const char *ssid, const char *password, IPAddress mqttHost, int mqttPort)
    : _ssid(ssid), _password(password), _mqttHost(mqttHost), _mqttPort(mqttPort)
{
}
void MQTTModule::connectToWifi()
{
    Serial.println("Connecting to Wi-Fi...");
    WiFi.begin(_ssid, _password);
}

void MQTTModule::connectToMqtt()
{
    Serial.println("Connecting to MQTT...");
    _mqttClient.connect();
}

void MQTTModule::onWifiConnect(const WiFiEventStationModeGotIP &event)
{
    Serial.println("Connected to Wi-Fi.");
    Serial.println(WiFi.localIP());
    connectToMqtt();
}

void MQTTModule::onWifiDisconnect(const WiFiEventStationModeDisconnected &event)
{
    Serial.println("Disconnected from Wi-Fi.");
    _mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    _wifiReconnectTimer.once(2, [this]()
                             { connectToWifi(); });
}

void MQTTModule::onMqttConnect(bool sessionPresent)
{
    Serial.println("Connected to MQTT.");
    Serial.print("Session present: ");
    Serial.println(sessionPresent);
}

void MQTTModule::onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
    Serial.println("Disconnected from MQTT.");

    if (WiFi.isConnected())
    {
        _mqttReconnectTimer.once(2, [this]()
                                 { connectToMqtt(); });
    }
}

void MQTTModule::onMqttPublish(uint16_t packetId)
{
    Serial.println("Publish acknowledged.");
    Serial.print("  packetId: ");
    Serial.println(packetId);
}

void MQTTModule::begin()
{
    WiFi.mode(WIFI_STA);
    _wifiConnectHandler = WiFi.onStationModeGotIP([this](const WiFiEventStationModeGotIP &event)
                                                  { onWifiConnect(event); });

    _wifiDisconnectHandler = WiFi.onStationModeDisconnected([this](const WiFiEventStationModeDisconnected &event)
                                                            {
        onWifiDisconnect(event);
        _mqttReconnectTimer.once(2 * 1000, [this]() {
            Serial.println("Reconnecting to Wi-Fi...");
            WiFi.begin(_ssid, _password);
        }); });

    _mqttClient.onConnect([this](bool sessionPresent)
                          { onMqttConnect(sessionPresent); });

    _mqttClient.onDisconnect([this](AsyncMqttClientDisconnectReason reason)
                             {
        onMqttDisconnect(reason);
        if (WiFi.isConnected()) {
            _mqttReconnectTimer.once(2 * 1000, [this]() {
                Serial.println("Reconnecting to MQTT...");
                _mqttClient.connect();
            });
        } });

    _mqttClient.onPublish([this](uint16_t packetId)
                          { onMqttPublish(packetId); });

    _mqttClient.setServer(_mqttHost, _mqttPort);
    connectToWifi();
}

void MQTTModule::publishData(const char *topic, float tdsValue, float phValue, float tempValue, float ecValue)
{
    char influxDBLineProtocol[128]; // Adjust buffer size as per your requirements
    sprintf(influxDBLineProtocol, "esp0,hydro_number=0 tds_value=%f,ph_value=%f,temp_value=%f,ec_value=%f", tdsValue, phValue, tempValue, ecValue);
    publish(topic, influxDBLineProtocol);
}

void MQTTModule::publish(const char *topic, const char *message)
{
    if (_mqttClient.connected())
    {
        _mqttClient.publish(topic, 2, true, message);
    }
}