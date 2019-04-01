#ifndef COMMON_LAMP_CONFIG_H_
#define COMMON_LAMP_CONFIG_H_

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <stdint.h>

#define BUTTON_PIN D7

typedef struct{
  uint32_t monitor_period;
  char mqtt_user[256];
  char mqtt_password[256];
  char mqtt_uri[256];
  uint32_t mqtt_port;
  char mqtt_topic[256];
  char device_name[128];
  uint32_t valid_config;
} config_t;


#endif
