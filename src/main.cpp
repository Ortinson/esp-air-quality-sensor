#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#define _TASK_STD_FUNCTION
#include <TaskScheduler.h>        //https://github.com/arkhipenko/TaskScheduler

#include "application/configuration_storage.h"
#include "application/configuration_server.h"

#define DEBUG

bool need_to_restart = false;

ConfigurationStorage* storage;
ConfigurationServer* server;
WiFiManager* wifiManager;

Scheduler scheduler;

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
    storage = new ConfigurationStorage();

    ConnectToWiFi(storage);

    server = new ConfigurationServer(storage);
    server->Start();
}

void loop() {
  scheduler.execute();
}
