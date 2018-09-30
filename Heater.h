#ifndef HEATER_H
#define HEATER_H
#include <DallasTemperature.h>
#include <OneWire.h>
#include "Log.h"

class Heater 
{
  private:
    const char* location;
    int relayPin;
    int sensorPin;
    DallasTemperature sensor;
    const uint8_t* deviceAddress;
    float offsetFloor = 4.0f;
    float temp;
    float maxFloor = 35.0f;
    int i=0;
    Log* logger;
    long previousMillis = 0;
    long interval = 2000;
    bool onState = false;
    int errors = 0;
  public:
    Heater();
    Heater(const char* f_location, int f_relayPin, int f_sensorPin, const uint8_t* f_deviceAddress, Log* f_logger);
    float readTemp();
    void relayOn();
    void relayOff();
    void turnOn();
    void turnOff();
    bool isOn();
    bool wantsToTurnOn();
    float getTemp();
    void handleHeating();
};

#endif
