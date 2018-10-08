#include "Log.h"
#define CLIENT_ID "Thermostat-log"

void callbackLog(char* topic, byte* payload, unsigned int length);

Log::Log() : Log(0) {
}

Log::Log(int f_level) {
  level = f_level;
  mqtt.setClient(espClient);
  mqtt.setServer("192.168.88.15", 1883);
  randomSeed(micros());
  mqtt.setCallback(callbackLog);
}

void Log::writeLog(const char* msg1, const char* msg2, int entryLevel) {
  if (level >= entryLevel) {
  char message[256] = {};
  switch(entryLevel)
  {
    case 0:
      strcat(message, "*LOG: [0] ");
      break;
    case 1:
      strcat(message, "*LOG: [1] ");
      break;
    case 2:
      strcat(message, "*LOG: [2] ");
      break;
    case 3:
      strcat(message, "*LOG: [3] ");
      break;
  }
  strcat(message, msg1);
  strcat(message, msg2);
  if(entryLevel!=0) 
    Serial.println(message);
    mqtt.publish("thermostat/log", message);
  } 
}

void Log::writeLog(const char* msg1, float number, int entryLevel) {
  char buffer[256] = {};
  snprintf(buffer, 256, "%f", number);
  writeLog(msg1, buffer, entryLevel);
}

void Log::updateState(const char* topic, const char* location, float number) {
  char buffer[64] = {};
  snprintf(buffer, 64, "%f", number);
  char topicToPublish[64] = {};
  snprintf(topicToPublish, 64, "thermostat/log/%s/%s", topic, location);
  mqtt.publish(topicToPublish, buffer);
}

void Log::updateState(const char* topic, const char* location, int number) {
  char buffer[64] = {};
  snprintf(buffer, 64, "%i", number);
  char topicToPublish[64] = {};
  snprintf(topicToPublish, 64, "thermostat/log/%s/%s", topic, location);
  mqtt.publish(topicToPublish, buffer);
}

void callbackLog(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void Log::reconnect() {
  // Loop until we're reconnected
  while (!mqtt.connected()) {
    writeLog("Logger is attempting MQTT connection...", "", 1);
    // Create a random client ID
    String clientId = "Thermostat-logger";
    // Attempt to connect
    if (mqtt.connect(clientId.c_str())) {
    writeLog("Logger connected to MQTT!", "", 1);
    } else {
      float mqttState = (float)mqtt.state();
    writeLog("Logger MQTT: failed, rc=", mqttState, 1);
    writeLog("Logger MQTT: try again next time", "", 1);
    }
  }
}

void Log::mqttLoop() {
  if(WiFi.status() == WL_CONNECTED){
    if (!mqtt.connected())
    {
        reconnect();
    }
    mqtt.loop();
  }
}

void Log::setlogLevel(int f_level) {
  level = f_level;
}
