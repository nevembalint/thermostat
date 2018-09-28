#include "Thermostat.h"

Thermostat::Thermostat(Log* f_logger)
{
  logger = f_logger;
  hallSensor = new Measure(13, logger, "hall");
  livingroom = new Room("livingroom", 0, 12, hallSensor, 24.0f, livingroomFloorAddress, logger);
  hall = new Room("hall", 4, 12, hallSensor, 24.0f, hallFloorAddress, logger);
  roomPriority[0] = hall;
  roomPriority[1] = livingroom;

}

bool Thermostat::managePriority(Room* room) {
  int i = 0;
  bool canTurnOn = true;
  while(roomPriority[i]->getRelayPin() != room->getRelayPin()) {
    if (roomPriority[i]->isOn()) {
      canTurnOn = false;
    } 
    i++;
  }

  if (canTurnOn && room->getHeater()->wantsToTurnOn() && !room->getOnState()) {
    logger->writeLog("Heat can turn on in ", room->getLocation(), 2);
    i++;
    while (i < sizeof(roomPriority)/sizeof(roomPriority[0])) {
      roomPriority[i]->heatOff();
      i++;
    }
    room->heatOn();
  } else {
    logger->writeLog("Heat can't turn on in ", room->getLocation(), 3);
  }
}

Log* Thermostat::getLogger() {
  return logger;
}

void Thermostat::handleHeating() {
  if (power) {
    for (int i = 0; i < sizeof(roomPriority)/sizeof(roomPriority[0]); i++) {
      Room* room = roomPriority[i];
      room->readTemp();
      if (room->getAirTemp() <= room->getMaxAirTemp() or room->isHeatingUp())
      {
        if (energySaver) {
        managePriority(room);
        } else {
          room->heatOn();
        }
      }
      if (room->getAirTemp() >= room->getMaxAirTemp() + offsetAir)
      {
        room->heatOff();
      }
      room->handleHeating();
    }
  } else {
    for (int i = 0; i < sizeof(roomPriority)/sizeof(roomPriority[0]); i++) {
      Room* room = roomPriority[i];
        room->heatOff();
    }
  }
  report();
}

float Thermostat::getMaxAirTemp() {
  return maxAirTemp;
}

void Thermostat::setMaxAirTemp(float f_temp) {
  maxAirTemp = f_temp;
  for (int i = 0; i < sizeof(roomPriority)/sizeof(roomPriority[0]); i++) {
      roomPriority[i]->setMaxAirTemp(maxAirTemp);
    }
}

void Thermostat::setEnergySaver(int f_en) {
  if (f_en==1)
    energySaver = true;
  if (f_en==0)
    energySaver = false;
}

void Thermostat::setPower(int f_power) {
  if (f_power==1)
    power = true;
  if (f_power==0)
    power = false;
}

void Thermostat::startUp() {
  float tempInEEPROM;
  EEPROM.get(maxAirTempMemLoc, tempInEEPROM);
  if ( tempInEEPROM != 0.0f) {
    setMaxAirTemp(tempInEEPROM);
    logger->writeLog("Temperature read from EEPROM and set: ", tempInEEPROM, 2);
  }
  int energySaverInEEPROM;
  EEPROM.get(energySaverMemLoc, energySaverInEEPROM);
    setEnergySaver(energySaverInEEPROM);
    logger->writeLog("EnergySaver option read from EEPROM and set: ", energySaverInEEPROM, 2);
  int powerInEEPROM;
  EEPROM.get(powerMemLoc, powerInEEPROM);
    setPower(powerInEEPROM);
    logger->writeLog("Power option read from EEPROM and set: ", powerInEEPROM, 2);
   if (!power) {
    logger->writeLog("Thermostat is off", "", 1);
   }
  int logLevelInEEPROM;
  EEPROM.get(logLevelMemLoc, logLevelInEEPROM);
    logger->setlogLevel(logLevelInEEPROM);
    logger->writeLog("LogLevel option read from EEPROM and set: ", logLevelInEEPROM, 2);
}

void Thermostat::report() {unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis; 
    logger->updateState("POWER", "all", power);
    logger->updateState("energySaver", "all", energySaver);
    for (int i = 0; i < sizeof(roomPriority)/sizeof(roomPriority[0]); i++) {
      const char* location = roomPriority[i]->getLocation();
      logger->updateState("maxAirTemp", location, roomPriority[i]->getMaxAirTemp());
      logger->updateState("airTemp", location, roomPriority[i]->getAirTemp());
      logger->updateState("floorTemp", location, roomPriority[i]->getFloorTemp());
      logger->updateState("humidity", location, roomPriority[i]->getHum());
      logger->updateState("relay", location, roomPriority[i]->isOn());
    }
  }
}
