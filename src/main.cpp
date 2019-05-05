#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <PubSubClient.h>

#include "common/config.h"
#include "application/configuration_storage.h"
#include "application/configuration_server.h"

bool need_to_restart = false;

ConfigurationStorage* storage;
ConfigurationServer* server;
WiFiManager* wifiManager;

void configModeCallback (WiFiManager *myWiFiManager) {
  need_to_restart = true;
}

void ConnectToWiFi(ConfigurationStorage* storage) {
  wifiManager = new WiFiManager();
  wifiManager->setAPCallback(configModeCallback);
  wifiManager->autoConnect(storage->GetStoredConfig()->device_name);
  if (need_to_restart)
    ESP.restart();
  Serial.printf("connected. IP: %s\n", WiFi.localIP().toString().c_str());
}

void setup() {
  Serial.begin(115200);
  Serial.printf("STARTING DEVICE./n");
  pinMode(BUTTON_PIN, INPUT_PULLUP);  
  
  storage = new ConfigurationStorage();
  config_t* config = storage->GetStoredConfig();
  if (!digitalRead(BUTTON_PIN)) {
    ConnectToWiFi(storage);
    Serial.printf("Configuration mode./n");
    server = new ConfigurationServer(storage);
    server->Start();
    return;
  }

  // make measurement
  double temp = 66.6;
  
  // Send data
  ConnectToWiFi(storage);
  WiFiClient espClient;
  PubSubClient client(espClient);
  client.setServer(config->mqtt_uri, config->mqtt_port);
  client.connect(config->device_name, config->mqtt_user, config->mqtt_password);
  client.publish(config->mqtt_topic, String(temp).c_str());
  client.loop(); //NO IDEA, probably needed to send the message
  Serial.printf("Data sent./n");

  ESP.deepSleep(config->monitor_period * 1e6);
}

void loop() {
}

// TODO: implement batery saving strategy when wifi network is not avaiable.
//   - Current situation the device stays as an active access point

// TODO: Reduce power consumption by:
  // - Having WIFI turned off by default
  // - Storing N data adquisitions before turning on wifi and sending to server

// TODO: Make sure data is sent correctly

// TODO: Implement battery level measurement