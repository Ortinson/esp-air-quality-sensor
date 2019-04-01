#include "configuration_storage.h"

ConfigurationStorage::ConfigurationStorage() {
  EEPROM.begin(sizeof(config_t));
  EEPROM.get(this->_eeprom_addr, this->_config);

  if(this->_config.valid_config != this->_default_config.valid_config) {
    Serial.println("Updating config to flash");
    this->_config = this->_default_config;
    this->StoreConfig(&this->_config);
  }
}

config_t* ConfigurationStorage::GetStoredConfig() {
  return &_config;
}

void ConfigurationStorage::StoreConfig(config_t* config) {
  this->_config = *config;
  EEPROM.put(this->_eeprom_addr, this->_config);
  EEPROM.commit();
  if(this->_callback) {
    this->_callback();
  }
}

void ConfigurationStorage::SetDeviceName(String conf) {
  conf.toCharArray(static_cast<char*>(this->_config.device_name), sizeof(this->_config.device_name));
  this->StoreConfig(&this->_config);
}
void ConfigurationStorage::SetMonitorPeriod(String conf) {
  this->_config.monitor_period = std::atoi(conf.c_str());
  this->StoreConfig(&this->_config);
}
void ConfigurationStorage::SetMqttPassword(String conf) {
  conf.toCharArray(static_cast<char*>(this->_config.mqtt_password), sizeof(this->_config.mqtt_password));
  this->StoreConfig(&this->_config);
}
void ConfigurationStorage::SetMqttUser(String conf) {
  conf.toCharArray(static_cast<char*>(this->_config.mqtt_user), sizeof(this->_config.mqtt_user));
  this->StoreConfig(&this->_config);
}
void ConfigurationStorage::SetMqttUri(String conf) {
  conf.toCharArray(static_cast<char*>(this->_config.mqtt_uri), sizeof(this->_config.mqtt_uri));
  this->StoreConfig(&this->_config);
}

void ConfigurationStorage::SubscribeToConfigChange(std::function<void(void)> callback) {
  this->_callback = callback;
}

