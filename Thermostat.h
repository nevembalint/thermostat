#ifndef THERMOSTAT_H
#define THERMOSTAT_H
#include "Room.h"
#include "Log.h"
#include <EEPROM.h>
#include "USER_SETTINGS.H"
#include "Measure.h"

class Thermostat {
  private:
    Log* logger;
    bool power = true;
    const uint8_t livingroomFloorAddress[8] = { 0x28, 0xFF, 0xF3, 0xD1, 0xB2, 0x17, 0x04, 0x3E };
    Room* livingroom;
    
    const uint8_t hallFloorAddress[8] = { 0x28, 0xFF, 0xA2, 0x59, 0xA2, 0x17, 0x05, 0xB7 };
    Room* hall;

    Measure* hallSensor;
    
    float offsetAir = 0.4f;
    float offsetFloor = 4.0f;
    Room* roomPriority[2] = {};
    bool energySaver = true;

    float maxAirTemp = 25.0f;
    long previousMillis = 0;
    long interval = 3000;  

    bool heatingUp=true;
    
  public:
    Thermostat(Log* f_logger);
    bool managePriority(Room* room);
    Log* getLogger();
    void handleHeating();
    void handleController();
    float getMaxAirTemp();
    void setMaxAirTemp(float t_temp);
    void setEnergySaver(int f_en);
    void setPower(int f_power);
    void startUp();
    int EEaddress = 0;
    int maxAirTempMemLoc = 0;
    int energySaverMemLoc = maxAirTempMemLoc + sizeof(float);
    int powerMemLoc = energySaverMemLoc + sizeof(float);
    int logLevelMemLoc = powerMemLoc + sizeof(float);
    void report();
};

#endif
