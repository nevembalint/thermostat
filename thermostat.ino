#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>
#include "Room.h"
#include "Log.h"
#include "Thermostat.h"
#include "Controller.h"

WiFiManager wm;
Log* logger = new Log(3);
Thermostat* thermostat = new Thermostat(logger);
Controller* controller = new Controller(thermostat, logger);
int wifiCounter = 0;

void callbackController(char* topic, byte* payload, unsigned int length) {
  controller->callback(topic, payload, length);
}
void setup() {
  
  Serial.begin(115200);
  Serial.setTimeout(2000);
  EEPROM.begin(32);
    //reset settings - wipe credentials for testing
    //wm.resetSettings();
    wm.setWiFiAutoReconnect(true);
    wm.setConnectTimeout(90);
    wm.setConfigPortalBlocking(false);
    if(wm.autoConnect("Thermostat")){
        logger->writeLog("connected...yeey :)", "", 1);
    }
    else {
        logger->writeLog("Configportal running", "", 1);
    }
    
  thermostat->startUp();
}

void loop() {
    wm.process();
    thermostat->handleHeating();
    if (WiFi.status() == WL_CONNECTED) {
      logger->mqttLoop();
      controller->mqttLoop();
    } else {
      wifiCounter++;
      Serial.println(wifiCounter);
    }
}
