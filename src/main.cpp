#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <MQTT.h>
#include <ArduinoJson.h>

#include "application/configuration_storage.h"
#include "application/rtc_storage.h"
#include "application/configuration_server.h"
#include "application/cactus_io_BME280_I2C.h"

bool need_to_restart = false;

ConfigurationStorage* storage;
RTCStorage* rtc_storage;
ConfigurationServer* server;
WiFiManager* wifiManager;
config_t* config;

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

  // Get data from sensor and store at rtcmemory
void Measure(){
  BME280_I2C bme;
  if (!bme.begin()) { 
    Serial.println("Could not find a valid BME280 sensor, check wiring!"); 
  }
  digitalWrite(14, HIGH);
  bme.readSensor();
  RTCStorage::rtc_data measurement;
  measurement.age=0;
  measurement.battery= analogRead(A0);
  measurement.humidity = bme.getHumidity();
  measurement.pressure = bme.getPressure_MB();
  measurement.temperature = bme.getTemperature_C();
  digitalWrite(14, LOW);

  rtc_storage->StoreRTCData(measurement);
}


void SetAge(std::vector<RTCStorage::rtc_data> *data){
  std::vector<RTCStorage::rtc_data>::iterator it;
  int i = 0;
  Serial.printf("Monitor period: %i\n",config->monitor_period);
  for(it = data->begin(); it != data->end(); it++){
    it->age = config->monitor_period * i;
    Serial.printf("%i\n",it->age);
    i++;
  }
}

String BuildMessage(){
  String message = "";
  std::vector<RTCStorage::rtc_data> data = rtc_storage->GetRTCData();
  SetAge(&data);
  const int capacity = JSON_ARRAY_SIZE(16) + 16 * JSON_OBJECT_SIZE(5);
  StaticJsonBuffer<capacity> jb;
  JsonArray& arr = jb.createArray();

  std::vector<RTCStorage::rtc_data>::iterator it;
  for(it = data.begin(); it != data.end(); it++){
    JsonObject& obj = arr.createNestedObject();
    obj["age"] = it->age;
    obj["battery"] = it->battery;
    obj["humidity"] = it->humidity;
    obj["pressure"] = it->pressure;
    obj["temperature"] = it->temperature;
  }
  arr.printTo(message);
  return message;
}

void SendData(){
  String message = BuildMessage();
  Serial.printf("%s\n", message.c_str());
  Serial.printf("%i\n", message.length());

  config_t* config = storage->GetStoredConfig();
  ConnectToWiFi(storage);

  WiFiClient espClient;
  MQTTClient client(2048);
  client.begin(config->mqtt_uri, config->mqtt_port, espClient);
  client.connect(config->device_name, config->mqtt_user, config->mqtt_password);
  client.publish(config->mqtt_topic, message.c_str(), false, 1);
  client.loop();
  delay(10);  //Problem if not included after client.loop(). Mentioned in library doc
  Serial.printf("Data sent./n");
}

void DebugVector(){
  Serial.printf("Vector magic.\n");
  std::vector<RTCStorage::rtc_data>::iterator it;
  std::vector<RTCStorage::rtc_data> data = rtc_storage->GetRTCData();
  Serial.printf("Vector size: %d\n", data.size());
  for(it = data.begin(); it != data.end(); it++){
    Serial.printf("%f, %i\n",it->temperature, it->battery);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.printf("STARTING DEVICE.\n");
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(14, OUTPUT);
  digitalWrite(14, LOW);

  storage = new ConfigurationStorage();
  rtc_storage = new RTCStorage();
  config = storage->GetStoredConfig();
  if (!digitalRead(BUTTON_PIN)) {
    ConnectToWiFi(storage);
    Serial.printf("Configuration mode.\n");
    server = new ConfigurationServer(storage);
    server->Start();
    return;
  }

  Serial.printf("Measuring.\n");
  Measure();

  if(rtc_storage->IsFull()){
    Serial.printf("Storage full\n");
    SendData();    
    rtc_storage->Clear();
  }
  
  DebugVector();

  ESP.deepSleep(config->monitor_period * 1e6);
}


void loop() {
}

// TODO: implement battery saving strategy when wifi network is not avaiable.
//   - Current situation the device stays as an active access point

// TODO: Reduce power consumption by:
  // - Having WIFI turned off by default
  // - DONE Storing N data adquisitions before turning on wifi and sending to server

// TODO: Implement battery level measurement

// DONE: Make sure data is sent correctly
