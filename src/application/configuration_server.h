#ifndef APPLICATION_CONFIGURATION_SERVER_H_
#define APPLICATION_CONFIGURATION_SERVER_H_

#include "../submodules/ESPAsyncWebServer/src/ESPAsyncWebServer.h"  // https://github.com/me-no-dev/ESPAsyncWebServer

#include "configuration_storage.h"
#include "common/config.h"
#include "common/utils.h"

class ConfigurationServer {
public:
  ConfigurationServer(ConfigurationStorage* storage);
  void Start();

private:
  ConfigurationStorage* _storage;
  config_t* _config;
  AsyncWebServer* _server;

  void SendIndex(AsyncWebServerRequest *request);
  void UpdateConfig(AsyncWebServerRequest *request);
  String IndexTemplateProcessor(const String& var);
  void setURIS();
};

#endif // APPLICATION_CONFIGURATION_SERVER_H_
