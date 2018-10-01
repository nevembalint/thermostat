#ifndef ROOM_H
#define ROOM_H
#include "Heater.h"
#include "DHT.h" //https://github.com/markruys/arduino-DHT
#include "Measure.h"
#include "Log.h"

class Room {
  private:
    Heater* heater;
    Log* logger;
    Measure* airSensor;
    const char* location;
    int relayPin;
    float airTemp = 21.0f;
    float humidity;
    float maxAirTemp;
    float offsetAir = 2.0;
    bool heatingUp = true;
    int i = 0;
  public:
    Room();
    Room(const char* f_location, int f_relayPin, int f_floorSensorPin, Measure* f_airSensor, float f_maxAirTemp, const uint8_t* f_deviceAddress, Log* f_logger);
    void readTemp();
    void heatOn();
    void heatOff();
    Heater* getHeater();
    int getRelayPin();
    float getAirTemp();
    float getHum();
    float getFloorTemp();
    float getMaxAirTemp();
    const char* getLocation();
    bool isOn();
    bool isHeatingUp();
    void handleHeating();
    void setHeatingUp(bool value);
    void setMaxAirTemp(float f_temp);
};

#endif
