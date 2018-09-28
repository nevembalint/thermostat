#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <PubSubClient.h>
#include "Arduino.h"
#include <ESP8266WiFi.h>
#include "Thermostat.h"
#include "Log.h"
#include <EEPROM.h>
#include "USER_SETTINGS.H"

class Controller {
  private:
    PubSubClient mqtt;
    WiFiClient espClient;
    Thermostat* thermostat;
    Log* logger;
    void handle();
    void reconnect();
    
  public:
    Controller(Thermostat* f_thermostat, Log* f_logger);
    void mqttLoop();
    void callback(char* topic, byte* payload, unsigned int length);
};

#endif
