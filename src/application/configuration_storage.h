#ifndef APPLICATION_CONFIGURATION_STORAGE_H_
#define APPLICATION_CONFIGURATION_STORAGE_H_

#include <string>
#include <EEPROM.h>
#include "common/utils.h"
#include "common/config.h"

class ConfigurationStorage {
public:
  ConfigurationStorage(void);
  config_t* GetStoredConfig(void);
  void StoreConfig(config_t* config);
  void SubscribeToConfigChange(std::function<void(void)> callback);
  void SetMonitorPeriod(String conf);
  void SetMqttUser(String conf);
  void SetMqttPassword(String conf);
  void SetMqttUri(String conf);
  void SetDeviceName(String conf);

private:
  static const int _eeprom_addr = 0;
  // TODO(Ortinson): Move to confifg file (SPIFFS).
  const config_t _default_config = {
    .monitor_period = (uint32_t)60,
    .mqtt_user = "",
    .mqtt_password = "",
    .mqtt_uri = "",
    .device_name = "Temperature Sensor",
    .valid_config = 0xDEADBEEF
  };
  config_t _config;
  std::function<void(void)> _callback = NULL;

};

#endif // APPLICATION_CONFIGURATION_STORAGE_H_
