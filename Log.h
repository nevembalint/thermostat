#ifndef LOG_H
#define LOG_H

#include <PubSubClient.h>
#include "Arduino.h"
#include <ESP8266WiFi.h>

class Log {
  private:
    int level;
    PubSubClient mqtt;
    WiFiClient espClient;
    void handle();
    void reconnect();
  public:
    Log();
    Log(int f_level);
    void writeLog(const char* msg1, const char* msg2, int entryLevel);
    void writeLog(const char* msg1, float number, int entryLevel);
    void updateState(const char* topic, const char* location, float number);
    void updateState(const char* topic, const char* location, int number);
    void mqttLoop();
    void setlogLevel(int f_level);
};

#endif
