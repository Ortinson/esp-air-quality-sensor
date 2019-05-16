#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino
#include "configuration_server.h"

ConfigurationServer::ConfigurationServer(ConfigurationStorage* storage)
  : _storage(storage) {
    this->_config = this->_storage->GetStoredConfig();
    this->_server = new AsyncWebServer(80);
    SPIFFS.begin();
    this->setURIS();
  }

void ConfigurationServer::setURIS() {
  this->_server->serveStatic("/style.css", SPIFFS, "/style.css").setDefaultFile("index.html");
  this->_server->on("/", AWS_HTTP_GET, std::bind(&ConfigurationServer::SendIndex, this, std::placeholders::_1));

  this->_server->on("/", AWS_HTTP_POST, std::bind(&ConfigurationServer::UpdateConfig, this, std::placeholders::_1));
}

void ConfigurationServer::SendIndex(AsyncWebServerRequest *request){
  request->send(SPIFFS, "/index.html", String(), false, std::bind(&ConfigurationServer::IndexTemplateProcessor, this, std::placeholders::_1));
}

void ConfigurationServer::UpdateConfig(AsyncWebServerRequest *request){
  #ifdef DEBUG
    int args = request->args();
    for(int i=0;i<args;i++){
      Serial.printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
    }
  #endif

  String param;
  if (request->hasParam("dev_name", true)) {
    param = request->getParam("dev_name", true)->value();
    this->_storage->SetDeviceName(param);
  }
  if (request->hasParam("monitor_period", true)) {
    param = request->getParam("monitor_period", true)->value();
    this->_storage->SetMonitorPeriod(param);
  }
  if (request->hasParam("mqtt_user", true)) {
    param = request->getParam("mqtt_user", true)->value();
    if (param)
      this->_storage->SetMqttUser(param);
  }
  if (request->hasParam("mqtt_password", true)) {
    param = request->getParam("mqtt_password", true)->value();
    if (param.length() != 0)
      this->_storage->SetMqttPassword(param);
  }
  if (request->hasParam("mqtt_uri", true)) {
    param = request->getParam("mqtt_uri", true)->value();
    this->_storage->SetMqttUri(param);
  }
  if (request->hasParam("mqtt_port", true)) {
    param = request->getParam("mqtt_port", true)->value();
    this->_storage->SetMqttPort(param);
  }
  if (request->hasParam("mqtt_topic", true)) {
    param = request->getParam("mqtt_topic", true)->value();
    this->_storage->SetMqttTopic(param);
  }
  
  this->SendIndex(request);
  delay(1000);
  ESP.restart();
}

String ConfigurationServer::IndexTemplateProcessor(const String& var) {
  if(var == "DEV_NAME")
    return String(this->_config->device_name);
  if(var == "MONITOR_PERIOD")
    return String(this->_config->monitor_period);
  if(var == "MQTT_URI")
    return String(this->_config->mqtt_uri);
  if(var == "MQTT_USER")
    return String(this->_config->mqtt_user);
  if(var == "MQTT_PORT")
    return String(this->_config->mqtt_port);
  if(var == "MQTT_TOPIC")
    return String(this->_config->mqtt_topic);
  return String();
}

void ConfigurationServer::Start(){
  this->_server->begin();
}
